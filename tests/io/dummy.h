#ifndef freevisa_tests_dummy_h_
#define freevisa_tests_dummy_h_

#include <visa.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void using_dummy_resource();

void dummy_reader_restart();
bool dummy_reader_isempty();
ViByte const *dummy_reader_read(ViUInt32 *count);
void dummy_reader_reset();

void dummy_writer_append(char const *data, unsigned int count, ViStatus ret);

#ifdef __cplusplus
}
#endif

#endif
