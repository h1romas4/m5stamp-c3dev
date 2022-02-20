#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);
uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);
#ifdef __cplusplus
}
#endif /* __cplusplus */
