#ifndef ASSEMBLER_H
#define ASSEMBLER_H

void assemble_program(const char *source);
void assemble_file(const char *filename);
void assemble_to_file(const char *src_file, const char *out_file);

#endif
