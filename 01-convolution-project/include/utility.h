#pragma once 

#include <stdio.h>
#include <string> 

struct OptionEntry
{
    int   option_number;
    std::string option_name;
};

void print_benchmark(int engine_code, double elapsed);
void print_err(const char *msg, int errcode);
void print_warn(const char *msg);
int safe_atoi(const char *s, int *out);
int safe_atox(const char *s, unsigned int *out);
bool is_hex_string(const char *s);

int read_param(const char *param_name,
               FILE *stream,
               const char *default_value,
               char *out, const size_t out_len);

int read_int(const char *param_name,
             FILE *stream,
             const int *default_value,
             int *out);

int read_size_t(const char *param_name,
                FILE *stream,
                const size_t *default_value,
                size_t *out);

int read_char(const char *param_name,
              FILE *stream,
              const char *default_value,
              char *out);
        
int read_hex(const char *param_name,
             FILE *stream,
             const unsigned int *default_value,
             unsigned int *out, const size_t out_len);
        
int read_hex_string(const char *param_name,
                    FILE *stream,
                    const char *default_value,
                    char *out, const size_t out_len);
            
int read_int_range(const char *param_name,
                   FILE *stream,
                   const int *default_value,
                   const int min,
                   const int max,
                   int *out);
            
int read_option(const char *param_name,
                struct OptionEntry *options,
                const size_t options_len,
                FILE *stream,
                const int *default_option,
                int *out);