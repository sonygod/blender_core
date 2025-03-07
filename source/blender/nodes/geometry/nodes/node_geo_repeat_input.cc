/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "BKE_compute_contexts.hh"
#include "BKE_scene.h"

#include "DEG_depsgraph_query.hh"

#include "UI_interface.hh"
#include "UI_resources.hh"

#include "NOD_geometry.hh"
#include "NOD_socket.hh"
#include "NOD_zone_socket_items.hh"

#include "node_geometry_util.hh"

namespace blender::nodes::node_geo_repeat_input_cc {

NODE_STORAGE_FUNCS(NodeGeometryRepeatInput);

static void node_declare_dynamic(const bNodeTree &tree,
                                 const bNode &node,
                                 NodeDeclaration &r_declaration)
{
  NodeDeclarationBuilder b{r_declaration};
  b.add_input<decl::Int>("Iterations").min(0).default_value(1);

  const NodeGeometryRepeatInput &storage = node_storage(node);
  const bNode *output_node = tree.node_by_id(storage.output_node_id);
  if (output_node != nullptr) {
    const NodeGeometryRepeatOutput &output_storage =
        *static_cast<const NodeGeometryRepeatOutput *>(output_node->storage);
    socket_declarations_for_repeat_items(output_storage.items_span(), r_declaration);
  }
}

static void node_init(bNodeTree * /*tree*/, bNode *node)
{
  NodeGeometryRepeatInput *data = MEM_cnew<NodeGeometryRepeatInput>(__func__);
  /* Needs to be initialized for the node to work. */
  data->output_node_id = 0;
  node->storage = data;
}

static bool node_insert_link(bNodeTree *ntree, bNode *node, bNodeLink *link)
{
  bNode *output_node = ntree->node_by_id(node_storage(*node).output_node_id);
  if (!output_node) {
    return true;
  }
  return socket_items::try_add_item_via_any_extend_socket<RepeatItemsAccessor>(
      *ntree, *node, *output_node, *link);
}

static void node_register()
{
  static bNodeType ntype;
  geo_node_type_base(&ntype, GEO_NODE_REPEAT_INPUT, "Repeat Input", NODE_CLASS_INTERFACE);
  ntype.initfunc = node_init;
  ntype.declare_dynamic = node_declare_dynamic;
  ntype.gather_link_search_ops = nullptr;
  ntype.insert_link = node_insert_link;
  node_type_storage(
      &ntype, "NodeGeometryRepeatInput", node_free_standard_storage, node_copy_standard_storage);
  nodeRegisterType(&ntype);
}
NOD_REGISTER_NODE(node_register)

}  // namespace blender::nodes::node_geo_repeat_input_cc
