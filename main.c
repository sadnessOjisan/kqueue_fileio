#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>

int main(void)
{
    int fd;
    int kq;
    struct kevent kev;
    int ret;
    struct timespec tspec;

    /* 初期化 */
    fd = 0;
    kq = 0;
    (void)memset(&kev, 0, sizeof(kev));
    ret = 0;
    (void)memset(&tspec, 0, sizeof(tspec));

    /* test.mdを監視するよう登録 */
    fd = open("./test.md", O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    kq = kqueue();
    if (kq == -1)
    {
        perror("kqueue");
        exit(1);
    }
    EV_SET(&kev, fd, EVFILT_VNODE,
           EV_ADD | EV_ENABLE | EV_CLEAR,
           NOTE_DELETE | NOTE_WRITE, 0, NULL);
    ret = kevent(kq, &kev, 1, NULL, 0, NULL);
    if (ret == -1)
    {
        perror("kevent");
        exit(1);
    }

    /* test.mdを監視 */
    while (1)
    {
        /* イベントリストにあるイベントの要素数を取得 */
        ret = kevent(kq, NULL, 0, &kev, 1, NULL);

        if (ret == -1)
        {
            perror("kevent");
            exit(1);
        }
        else if (ret > 0)
        {
            if (kev.ident == fd &&
                (kev.fflags & NOTE_WRITE) == NOTE_WRITE)
            {
                printf("test.md is written\n");
            }
        }
    }
    
    /* イベントリストにあるイベントの要素数が0のときはループが回り続ける */
    
    return 0;
}
