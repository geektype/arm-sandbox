#ifndef FDT_H
#define FDT_H

#include "stdint.h"

#define FDT_START 0x40000000
#define FDT_MAX_SIZE 0x10000000
#define FDT_MAGIC 0xd00dfeed
#define FDT_COMPATIBLE_VERSION 17

#define FDT_HEADER_MAGIC_OFFSET 0x0
#define FDT_HEADER_TOTALSIZE_OFFSET 0x1
#define FDT_HEADER_STRUCT_OFFSET 0x2
#define FDT_HEADER_STRINGS_OFFSET 0x3
#define FDT_HEADER_MEM_OFFSET 0x4
#define FDT_HEADER_VERSION_OFFSET 0x5
#define FDT_HEADER_COMP_VERSION_OFFSET 0x6
#define FDT_HEADER_CPUID_OFFSET 0x7
#define FDT_HEADER_STRING_S_OFFSET 0x8
#define FDT_HEADER_STRUCT_S_OFFSET 0x9

#define FDT_MAX_NODES 50
#define FDT_MAX_PROPS_PER_NODE 5
#define FDT_MAX_NAME_LENGTH 32
#define FDT_MAX_PROP_SIZE 256

#define FDT_SUCCESS 0
#define FDT_ERR_INVALID_MAGIC 1
#define FDT_ERR_TOO_BIG 2
#define FDT_ERR_INCOMPATIBLE_VERSION 3
#define FDT_ERR_MAX_NODE_REACHED 4

struct fdt_header {
  uint32_t totalsize;
  uint32_t *struct_block;
  uint32_t *string_block;
  uint32_t *mem_block;
  uint32_t version;
  uint32_t last_comp_version;
  uint32_t boot_cpuid_phys;
  uint32_t size_dt_strings;
  uint32_t size_dt_struct;
};

enum fdt_token {
  FDT_BEGIN_NODE = 1,
  FDT_END_NODE = 2,
  FDT_PROP = 3,
  FDT_NOP = 4,
  FDT_END = 9
};

struct fdt_property {
  char name[FDT_MAX_NAME_LENGTH];
  uint8_t data[FDT_MAX_PROP_SIZE];
  uint32_t len;
};

struct dt_node {
  char name[FDT_MAX_NAME_LENGTH];
  uint32_t parent_idx;
  uint32_t first_child_idx;
  uint32_t next_sibling_idx;
  struct fdt_property properties[FDT_MAX_PROPS_PER_NODE];
  uint32_t num_properties;
};

struct dt_state {
  struct dt_node nodes[FDT_MAX_NODES];
  uint16_t num_nodes;
  uint16_t current_node_idx;
};

void dt_init(struct dt_state *state);
uint32_t alloc_node(struct dt_state *state);
int parse_fdt_header(uint32_t *fdt_start, struct fdt_header *header);
struct dt_state *parse_device_tree(struct fdt_header *header,
                                   struct dt_state *state);
void walk_dtb(struct fdt_header *header);
int fdt_get_alias(struct fdt_header *header, char *key, char *val);
int fdt_get_node_prop(struct fdt_header *header, char *path, char *prop,
                      const void **val);

#endif
