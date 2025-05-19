#include "common.h" // IWYU pragma: keep
#include "fdt.h"
#include "stdint.h"

int main(void) {
  struct fdt_header header;
  struct dt_state state;
  uint32_t *fdt_start = (uint32_t *)FDT_START;

  int h_res = parse_fdt_header(fdt_start, &header);

  if (h_res != FDT_SUCCESS)
    return h_res;

  // dt_init(&state);
  // struct dt_node *t = parse_device_tree(&header, &state);

  walk_dtb(&header);

  char path[20] = {0};
  // int res = fdt_get_alias(&header, "serial0", path);

  int rest =
      fdt_get_node_prop(&header, "aliases", "serial0", (const void **)path);

  return 0;
}
