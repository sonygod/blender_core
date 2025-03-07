/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup eevee
 */

#include "BKE_image.h"
#include "BKE_lib_id.h"
#include "BKE_node.hh"
#include "BKE_studiolight.h"

#include "NOD_shader.h"

#include "eevee_instance.hh"

namespace blender::eevee {

/* -------------------------------------------------------------------- */
/** \name Viewport Override Node-Tree
 * \{ */

LookdevWorldNodeTree::LookdevWorldNodeTree()
{
  bNodeTree *ntree = ntreeAddTree(nullptr, "Lookdev World Nodetree", ntreeType_Shader->idname);
  ntree_ = ntree;

  bNode *coordinate = nodeAddStaticNode(nullptr, ntree, SH_NODE_TEX_COORD);
  bNodeSocket *coordinate_out = nodeFindSocket(coordinate, SOCK_OUT, "Generated");

  bNode *rotate = nodeAddStaticNode(nullptr, ntree, SH_NODE_VECTOR_ROTATE);
  rotate->custom1 = NODE_VECTOR_ROTATE_TYPE_AXIS_Z;
  bNodeSocket *rotate_vector_in = nodeFindSocket(rotate, SOCK_IN, "Vector");
  angle_socket_ = static_cast<bNodeSocketValueFloat *>(
      static_cast<void *>(nodeFindSocket(rotate, SOCK_IN, "Angle")->default_value));
  bNodeSocket *rotate_out = nodeFindSocket(rotate, SOCK_OUT, "Vector");

  bNode *environment = nodeAddStaticNode(nullptr, ntree, SH_NODE_TEX_ENVIRONMENT);
  environment_node_ = environment;
  NodeTexImage *environment_storage = static_cast<NodeTexImage *>(environment->storage);
  bNodeSocket *environment_vector_in = nodeFindSocket(environment, SOCK_IN, "Vector");
  bNodeSocket *environment_out = nodeFindSocket(environment, SOCK_OUT, "Color");

  bNode *background = nodeAddStaticNode(nullptr, ntree, SH_NODE_BACKGROUND);
  bNodeSocket *background_out = nodeFindSocket(background, SOCK_OUT, "Background");
  bNodeSocket *background_color_in = nodeFindSocket(background, SOCK_IN, "Color");
  intensity_socket_ = static_cast<bNodeSocketValueFloat *>(
      static_cast<void *>(nodeFindSocket(background, SOCK_IN, "Strength")->default_value));

  bNode *output = nodeAddStaticNode(nullptr, ntree, SH_NODE_OUTPUT_WORLD);
  bNodeSocket *output_in = nodeFindSocket(output, SOCK_IN, "Surface");

  nodeAddLink(ntree, coordinate, coordinate_out, rotate, rotate_vector_in);
  nodeAddLink(ntree, rotate, rotate_out, environment, environment_vector_in);
  nodeAddLink(ntree, environment, environment_out, background, background_color_in);
  nodeAddLink(ntree, background, background_out, output, output_in);
  nodeSetActive(ntree, output);

  /* Create a dummy image data block to hold GPU textures generated by studio-lights. */
  STRNCPY(image.id.name, "IMLookdev");
  BKE_libblock_init_empty(&image.id);
  image.type = IMA_TYPE_IMAGE;
  image.source = IMA_SRC_GENERATED;
  ImageTile *base_tile = BKE_image_get_tile(&image, 0);
  base_tile->gen_x = 1;
  base_tile->gen_y = 1;
  base_tile->gen_type = IMA_GENTYPE_BLANK;
  copy_v4_fl(base_tile->gen_color, 0.0f);
  /* TODO: This works around the issue that the first time the texture is accessed the image would
   * overwrite the set GPU texture. A better solution would be to use image data-blocks as part of
   * the studio-lights, but that requires a larger refactoring. */
  BKE_image_get_gpu_texture(&image, &environment_storage->iuser, nullptr);
}

LookdevWorldNodeTree::~LookdevWorldNodeTree()
{
  ntreeFreeEmbeddedTree(ntree_);
  MEM_SAFE_FREE(ntree_);
  BKE_libblock_free_datablock(&image.id, 0);
}

bNodeTree *LookdevWorldNodeTree::nodetree_get(const LookdevParameters &parameters)
{
  intensity_socket_->value = parameters.intensity;
  angle_socket_->value = parameters.rot_z;

  GPU_TEXTURE_FREE_SAFE(image.gputexture[TEXTARGET_2D][0]);
  environment_node_->id = nullptr;

  StudioLight *sl = BKE_studiolight_find(parameters.hdri.c_str(),
                                         STUDIOLIGHT_ORIENTATIONS_MATERIAL_MODE);
  if (sl) {
    BKE_studiolight_ensure_flag(sl, STUDIOLIGHT_EQUIRECT_RADIANCE_GPUTEXTURE);
    GPUTexture *texture = sl->equirect_radiance_gputexture;
    if (texture != nullptr) {
      GPU_texture_ref(texture);
      image.gputexture[TEXTARGET_2D][0] = texture;
      environment_node_->id = &image.id;
    }
  }

  return ntree_;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name Lookdev
 *
 * \{ */

LookdevModule::~LookdevModule()
{
  GPU_material_free(&gpu_materials_);
  gpu_material_ = nullptr;
}

bool LookdevModule::sync_world()
{
  /* Check based on the v3d if the world is overridden. */
  LookdevParameters new_parameters(inst_.v3d);
  if (parameters_ != new_parameters) {
    if (parameters_.gpu_parameters_changed(new_parameters)) {
      GPU_material_free(&gpu_materials_);
      gpu_material_ = nullptr;
    }

    parameters_ = new_parameters;
    inst_.reflection_probes.do_world_update_set(true);
    inst_.sampling.reset();
  }

  if (parameters_.show_scene_world) {
    return false;
  }

  ::bNodeTree *node_tree = world_override_tree.nodetree_get(parameters_);
  gpu_material_ = inst_.shaders.material_shader_get("EEVEE Lookdev Background",
                                                    gpu_materials_,
                                                    node_tree,
                                                    MAT_PIPE_DEFERRED,
                                                    MAT_GEOM_WORLD,
                                                    true);
  inst_.pipelines.world.sync(gpu_material_);
  inst_.pipelines.background.sync(gpu_material_, parameters_.background_opacity);

  return true;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name Parameters
 * \{ */

LookdevParameters::LookdevParameters() {}

LookdevParameters::LookdevParameters(const ::View3D *v3d)
{
  if (v3d == nullptr) {
    return;
  }

  const ::View3DShading &shading = v3d->shading;
  show_scene_world = shading.type == OB_RENDER ? shading.flag & V3D_SHADING_SCENE_WORLD_RENDER :
                                                 shading.flag & V3D_SHADING_SCENE_WORLD;
  if (!show_scene_world) {
    rot_z = shading.studiolight_rot_z;
    background_opacity = shading.studiolight_background;
    blur = shading.studiolight_blur;
    intensity = shading.studiolight_intensity;
    hdri = StringRefNull(shading.lookdev_light);
  }
}

bool LookdevParameters::operator==(const LookdevParameters &other) const
{
  return hdri == other.hdri && rot_z == other.rot_z &&
         background_opacity == other.background_opacity && blur == other.blur &&
         intensity == other.intensity && show_scene_world == other.show_scene_world;
}

bool LookdevParameters::gpu_parameters_changed(const LookdevParameters &other) const
{
  return !(hdri == other.hdri && rot_z == other.rot_z && blur == other.blur &&
           intensity == other.intensity);
}

bool LookdevParameters::operator!=(const LookdevParameters &other) const
{
  return !(*this == other);
}

/** \} */

}  // namespace blender::eevee
