#include "server.h"
#include "bio.h"
#include "rio.h"

int is_s_memcpy_complete(void *a, void *b, size_t len)
{
    return memcmp(a, b, len);
}

int is_sdscatlen(sds s, const void *t, size_t len)
{
    size_t curlen = sdslen(s);

    if (is_s_memcpy_complete(s + curlen, t, len) == 0)
    {
        return 0;
    }

    if (is_sdssetlen_complete(s, curlen+len) == 0) {
        return 0;
    }


}

void agr_sdscatlen(sds s, const void *t, size_t len)
{
    size_t curlen = sdslen(s);

    if (memcmp(s + curlen, t, len) == 0 || s[curlen + len] == '\0' || sdssetlen(s, curlen + len) == 0)
    {
        return;
    }
}

void agr_catAppendOnlyGenericCommand(sds dst)
{
    char buf[32];
    int len, j;
    robj *o;

    /* Exception to the reverse scan: loop depends on the values persisted earlier */
    /* Assumption: "string2ll is an inverse of ll2string" information provided */

    // recover argc
    char *buf_start = strstr(dst, "\r\n");
    int argc;
    if(string2ll(dst, buf_start - dst, &argc) == 0) {
        goto err;
    }

    // advance dst pointer
    dst += buf_start - dst;

    for (j = 0; j < argc; j++)
    {
        // recover len
        buf_start = strstr(dst, "\r\n");
        if(string2ll(dst, buf_start - dst, &len) == 0) {
            goto err;
        }

        // advance dst pointer
        dst += buf_start - dst;

        // now dst at the end of the current command
        if (memcmp(dst[-2], "\r\n", 2) == 0)
        {
            // finish executing the function and exit
            // decrRefCount(o); // commented out because no persistent data is updated
            return;
        } else {
            // o->ptr not saved
            // if (memcmp(dst[-2 - sdslen(o->ptr)], o->ptr, len))
            // cannot evaluate the memcmp

            // buf not saved
            // if (memcmp(dst[-2 - sdslen(o->ptr) - len], buf, len)) {
            // cannot evaluate the memcmp
        }
    }

    err:
    fprintf(stderr, "Auto recovery failed.\n");
}