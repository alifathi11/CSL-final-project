#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include "utility.h"
#include "constants.h"

void print_err(const char *msg, int errcode) {
    fprintf(stderr, "%s %s (CODE: %d)\n", LOG_LEVEL_ERROR, msg, errcode);
}

int safe_atoi(const char *s, int *out)
{
    if (!s || !out)
        return -1;

    char *endptr = NULL;
    errno = 0;
    const long v = strtol(s, &endptr, 10);

    if (errno != 0)
        return -1;

    while (isspace((unsigned char)*endptr))
        endptr++;

    if (endptr == s || *endptr != '\0')
        return -1;

    if (v < INT_MIN || v > INT_MAX)
        return -1;

    *out = (int)v;
    return 0;
}

int safe_atox(const char *s, unsigned int *out)
{
    char *end = NULL;
    errno = 0;

    const unsigned long v = strtoul(s, &end, 16);

    if (errno != 0 || end == s || *end != '\0')
    {
        return -1;
    }

    if (v > UINT_MAX)
    {
        return -1;
    }

    *out = (unsigned int) v;
    return 0;
}

int is_hex_string(const char *s)
{
    if (!s) return 0;
    const size_t L = strlen(s);
    if (L == 0 || (L % 2) != 0) return 0;
    for (size_t i = 0; i < L; ++i)
    {
        if (!isxdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

int read_param(const char *param_name,
               FILE *stream,
               const char *default_value,
               char *out, const size_t out_len)
{
    if ((!param_name || !stream || !default_value || !out) || out_len == 0)
        return CODE_FAILURE_INVALID_ARG;

    printf("Enter %s [%s] > ", param_name, default_value);
    fflush(stdout);

    memset(out, 0, out_len);

    if (fgets(out, (int)out_len, stream) == NULL)
        return CODE_FAILURE_READ_INPUT;

    size_t len = strlen(out);
    if (len > 0 && out[len - 1] == '\n')
        out[len - 1] = '\0';
    else
        while ((len = fgetc(stream)) != '\n' && (int) len != EOF) { }

    if (out[0] == '\0')
        strncpy(out, default_value, out_len - 1);

    out[out_len - 1] = '\0';
    return CODE_SUCCESS;
}


int read_int(const char *param_name,
             FILE *stream,
             const int *default_value,
             int *out)
{
    if (!param_name || !stream || !default_value || !out)
    {
        fprintf(stderr, "[Error] Invalid arguments to read int\n");
        return CODE_FAILURE_INVALID_ARG;
    }

    char buf[MED_BUF_SIZE];
    char defbuf[MED_BUF_SIZE];

    const char *defstr = NULL;
    snprintf(defbuf, sizeof(defbuf), "%d", *default_value);
    defstr = defbuf;


    const int res = read_param(param_name, stream, defstr, buf, sizeof(buf));
    if (res != CODE_SUCCESS)
    {
        return res;
    }

    if (safe_atoi(buf, out) != CODE_SUCCESS)
    {
        return CODE_FAILURE;
    }

    return CODE_SUCCESS;
}

int read_size_t(const char *param_name,
                FILE *stream,
                const size_t *default_value,
                size_t *out)
{
    if (!param_name || !stream || !default_value || !out)
    {
        fprintf(stderr, "[Error] Invalid arguments to read size_t\n");
        return CODE_FAILURE_INVALID_ARG;
    }

    char buf[MED_BUF_SIZE];
    char defbuf[MED_BUF_SIZE];

    const char *defstr = NULL;
    snprintf(defbuf, sizeof(defbuf), "%zu", *default_value);
    defstr = defbuf; 

    const int res = read_param(param_name, stream, defstr, buf, sizeof(buf));
    if (res != CODE_SUCCESS)
    {
        return res;
    }
    char *end = NULL;
    errno = 0;
    const unsigned long v = strtoul(buf, &end, 10);
    if (errno != 0 || end == buf || *end != '\0')
    {
        return CODE_FAILURE;
    }

    *out = (size_t)v;
    return CODE_SUCCESS;
}

int read_char(const char *param_name,
              FILE *stream,
              const char *default_value,
              char *out)
{
    if ((!param_name || !stream || !default_value || !out))
    {
        fprintf(stderr, "[Error] Invalid arguments to read char\n");
        return CODE_FAILURE_INVALID_ARG;
    }

    char buf[MED_BUF_SIZE];

    const int res = read_param(param_name, stream, default_value, buf, sizeof(buf));
    if (res != CODE_SUCCESS)
    {
        return res;
    }

    *out = buf[0];
    return CODE_SUCCESS;
}

int read_hex(const char *param_name,
             FILE *stream,
             const unsigned int *default_value,
             unsigned int *out, const size_t out_len)
{
    if ((!param_name || !stream || !default_value || !out) ||
        (out_len <= 0))
    {
        fprintf(stderr, "[Error] Invalid arguments to read hex\n");
        return CODE_FAILURE_INVALID_ARG;
    }

    char buf[MED_BUF_SIZE];
    char defbuf[MED_BUF_SIZE];

    const char *defstr = NULL;
    snprintf(defbuf, sizeof(defbuf), "%X", *default_value);
    defstr = defbuf;


    const int res = read_param(param_name, stream, defstr, buf, sizeof(buf));
    if (res != CODE_SUCCESS)
    {
        return res;
    }

    if (safe_atox(buf, out) != CODE_SUCCESS)
    {
        return CODE_FAILURE;
    }
    return CODE_SUCCESS;
}

int read_hex_string(const char *param_name,
                    FILE *stream,
                    const char *default_value,
                    char *out, const size_t out_len)
{
    if ((!param_name || !stream || !default_value || !out) ||
        (out_len <= 0))
    {
        fprintf(stderr, "[Error] Invalid arguments to read hex string\n");
        return CODE_FAILURE_INVALID_ARG;
    }

    const int res = read_param(param_name, stream, default_value, out, out_len);
    if (res != CODE_SUCCESS)
    {
        return res;
    }

    if (!is_hex_string(out))
    {
        return CODE_FAILURE_INVALID_INPUT;
    }

    return CODE_SUCCESS;
}


int read_int_range(const char *param_name,
                   FILE *stream,
                   const int *default_value,
                   const int min,
                   const int max,
                   int *out)
{
    if (!param_name || !stream || !default_value || !out)
    {
        fprintf(stderr, "[Error] Invalid arguments to read int range\n");
        return CODE_FAILURE_INVALID_ARG;
    }

    int v;

    const int res = read_int(param_name, stream, default_value, &v) ;
    if (res != CODE_SUCCESS)
    {
        return res;
    }

    if (v < min || v > max)
    {
        return CODE_FAILURE_OUT_OF_RANGE;
    }

    *out = v;
    return CODE_SUCCESS;
}

int read_option(const char *param_name,
                struct OptionEntry *options,
                const size_t options_len,
                FILE *stream,
                const int *default_option,
                int *out)
{
    if ((!param_name || !options || !stream || !default_option || !out) ||
        (options_len <= 0))
    {
        fprintf(stderr, "[Error] Invalid arguments to read option\n");
        return CODE_FAILURE_INVALID_ARG;
    }


    fprintf(stderr, "Options:\n");
    for (size_t i = 0; i < options_len; i++)
    {
        printf("\t(%d) %s\n", options[i].option_number, options[i].option_name.c_str());
    }

    int selected_option;
    const int res = read_int(param_name, stdin, default_option, &selected_option);

    if (res != CODE_SUCCESS)
    {
        return res;
    }

    int valid = 0;
    for (size_t i = 0; i < options_len; i++)
    {
        if (selected_option == options[i].option_number)
        {
            valid = 1;
            break;
        }
    }

    if (!valid)
    {
        fprintf(stderr, "[Error] Invalid number (%d) restored to default value (%d).\n", selected_option, *default_option);
        selected_option = *default_option;
    }

    *out = selected_option;
    return CODE_SUCCESS;
}