char buf[12];

void f2str(float n, char *a){
    n *= 100;
    int nn = n > 0 ? (int)n : ((int)n) * (-1);
    int i = 10;
    while (nn > 0){
        a[i--] = nn % 10 + 48;
        nn /= 10;
        if (i == 8)
            a[i--] = '.';
    }
    if (i == 7) a[i--] = 48;
    if (n < 0)  a[i--] = '-';
    while (nn <= 10)
        a[nn++] = i < 10 ? a[++i] : 48;
}
