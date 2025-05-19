#include "fdt.h"
#include "common.h" // IWYU pragma: keep
#include "stddef.h"
#include "stdint.h"
#include "string.h"

uint32_t swap_endian(uint32_t value) {
  return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) |
         ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24);
}

void dt_init(struct dt_state *state) {
  state->num_nodes = 0;
  state->current_node_idx = 0;
}

uint32_t alloc_node(struct dt_state *state) {
  if (state->num_nodes >= FDT_MAX_NODES) {
    return UINT32_MAX;
  }

  uint32_t idx = state->num_nodes++;
  state->nodes[idx].parent_idx = UINT32_MAX;
  state->nodes[idx].first_child_idx = UINT32_MAX;
  state->nodes[idx].next_sibling_idx = UINT32_MAX;
  state->nodes[idx].num_properties = 0;

  return idx;
}

int parse_fdt_header(uint32_t *fdt_start, struct fdt_header *header) {
  uint32_t magic = swap_endian(fdt_start[FDT_HEADER_MAGIC_OFFSET]);
  if (magic != FDT_MAGIC)
    return FDT_ERR_INVALID_MAGIC;

  header->totalsize = swap_endian(fdt_start[FDT_HEADER_TOTALSIZE_OFFSET]);
  if (header->totalsize > FDT_MAX_SIZE)
    return FDT_ERR_TOO_BIG;

  uint32_t struct_offset = swap_endian(fdt_start[FDT_HEADER_STRUCT_OFFSET]);
  uint32_t string_offset = swap_endian(fdt_start[FDT_HEADER_STRINGS_OFFSET]);
  uint32_t mem_offset = swap_endian(fdt_start[FDT_HEADER_MEM_OFFSET]);

  header->struct_block = (uint32_t *)((uint8_t *)fdt_start + struct_offset);
  header->string_block = (uint32_t *)((uint8_t *)fdt_start + string_offset);
  header->mem_block = (uint32_t *)((uint8_t *)fdt_start + mem_offset);

  header->version = swap_endian(fdt_start[FDT_HEADER_VERSION_OFFSET]);
  header->last_comp_version =
      swap_endian(fdt_start[FDT_HEADER_COMP_VERSION_OFFSET]);
  if (header->version != 17 || header->last_comp_version != 16)
    return FDT_ERR_INCOMPATIBLE_VERSION;

  header->boot_cpuid_phys = swap_endian(fdt_start[FDT_HEADER_CPUID_OFFSET]);
  header->size_dt_strings = swap_endian(fdt_start[FDT_HEADER_STRING_S_OFFSET]);
  header->size_dt_struct = swap_endian(fdt_start[FDT_HEADER_STRUCT_S_OFFSET]);

  return FDT_SUCCESS;
}

void clear_string(char str[], size_t size) {
  for (size_t i = 0; i < size; ++i) {
    str[i] = '\0';
  }
}

int fdt_get_alias(struct fdt_header *header, char *key, char *val) {
  uint32_t token;

  uint32_t *p = header->struct_block;
  char current_node[20] = {0};

  while (1) {
    token = swap_endian(*p);
    p++;

    switch (token) {
    case FDT_BEGIN_NODE: {
      const char *name = (const char *)p;
      uint32_t len = k_strlen(name) + 1;
      clear_string(current_node, sizeof(current_node));
      k_strcpy(current_node, name);
      uint32_t len_aligned = (len + 3) & ~3;
      p += len_aligned / 4;
      break;
    }
    case FDT_PROP: {
      uint32_t len = swap_endian(p[0]);
      uint32_t nameoff = swap_endian(p[1]);
      const char *prop_name =
          (char *)(((uint8_t *)header->string_block) + nameoff);
      if (k_strcmp(current_node, "aliases") == 0) {
        if (k_strcmp(prop_name, key) == 0) {
          char *value = (char *)(p + 2);
          k_strcpy(val, value);
          return 0;
        }
      }

      // const void *value = (const void *)(p + 2);
      size_t val_aligned = (len + 3) & ~3;
      p += 2 + (val_aligned / 4);
      break;
    }
    case FDT_END_NODE: {
      break;
    }
    case FDT_NOP: {
      break;
    }
    case FDT_END: {
      return -1;
    }
    default: {
      return -1;
    }
    }
  }

  return -1;
}

int fdt_get_node_prop(struct fdt_header *header, char *path, char *prop,
                      const void **val) {
  uint32_t token;

  uint32_t *p = header->struct_block;
  char current_node[21] = {0};

  while (1) {
    token = swap_endian(*p);
    p++;

    switch (token) {
    case FDT_BEGIN_NODE: {
      const char *name = (const char *)p;
      uint32_t len = k_strlen(name) + 1;
      clear_string(current_node, sizeof(current_node));
      k_strcpy(current_node, name);
      uint32_t len_aligned = (len + 3) & ~3;
      p += len_aligned / 4;
      break;
    }
    case FDT_PROP: {
      uint32_t len = swap_endian(p[0]);
      uint32_t nameoff = swap_endian(p[1]);
      const char *prop_name =
          (char *)(((uint8_t *)header->string_block) + nameoff);
      if (k_strcmp(current_node, path) == 0) {
        if (k_strcmp(prop_name, prop) == 0) {
          *val = (const void *)(p + 2);
          return 0;
        }
      }

      size_t val_aligned = (len + 3) & ~3;
      p += 2 + (val_aligned / 4);
      break;
    }
    case FDT_END_NODE: {
      break;
    }
    case FDT_NOP: {
      break;
    }
    case FDT_END: {
      return -1;
    }
    default: {
      return -1;
    }
    }
  }

  return -1;
}

void walk_dtb(struct fdt_header *header) {
  uint32_t token;

  uint32_t *p = header->struct_block;

  uint32_t node_count = 0;
  uint32_t prop_count = 0;

  char current_node[20] = {0};

  while (1) {
    token = swap_endian(*p);
    p++;

    switch (token) {
    case FDT_BEGIN_NODE: {
      const char *name = (const char *)p;
      uint32_t len = k_strlen(name) + 1;
      clear_string(current_node, sizeof(current_node));
      k_strcpy(current_node, name);
      uint32_t len_aligned = (len + 3) & ~3;
      p += len_aligned / 4;
      node_count++;
      break;
    }
    case FDT_PROP: {
      uint32_t len = swap_endian(p[0]);
      uint32_t nameoff = swap_endian(p[1]);
      const char *prop_name =
          (char *)(((uint8_t *)header->string_block) + nameoff);
      if (k_strcmp(current_node, "aliases") == 0) {
        if (k_strcmp(prop_name, "serial0") == 0) {
          continue;
        }
      }

      const void *value = (const void *)(p + 2);
      size_t val_aligned = (len + 3) & ~3;
      p += 2 + (val_aligned / 4);
      prop_count++;
      break;
    }
    case FDT_END_NODE: {
      break;
    }
    case FDT_NOP: {
      break;
    }
    case FDT_END: {
      return;
    }
    default: {
      return;
    }
    }
  }

  return;
}

struct dt_state *parse_device_tree(struct fdt_header *header,
                                   struct dt_state *state) {

  uint32_t *dt_struct = header->struct_block;

  struct dt_node *root = NULL;
  struct dt_node *current = NULL;

  uint32_t token;
  uint32_t offset = 0;

  while (offset < header->size_dt_struct) {
    token = swap_endian(dt_struct[offset]);
    offset += 1;

    switch (token) {
    case FDT_BEGIN_NODE: {
      const char *name = (const char *)(dt_struct + offset);
      uint32_t namelen = k_strlen(name) + 1;
      offset += (namelen + 3) >> 2;

      uint32_t new_idx = alloc_node(state);
      if (new_idx == UINT32_MAX) {
        return NULL;
      }

      // check if name is empty
      if (name[0] == '\0') {
        k_strcpy(state->nodes[new_idx].name, "/");
      }
      // set name

      break;
    }

    default:
      return NULL;
    }
  }
  return NULL;
}
