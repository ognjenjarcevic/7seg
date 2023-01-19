
Drajver:
    make                        -Kreiranje
Info o modulu.
    modinfo gpio_bitbang.ko
Provera ucitanosti
    lsmod
Ucitavanje
    sudo insmod gpio_bitbang.ko      
    sudo rm -f /dev/gpio_bitbang
    sudo mknod /dev/gpio_bitbang c 260 0
    sudo chmod a+rw /dev/gpio_bitbang
Brisanje
    sudo rmmod gpio_bitbang.ko

Pracenje poruka
    dmesg --follow


Displej bez drajvera:
    Kompajliranje:
        gcc -o displej main.c display.c bcm2835.c circular_buffer.c -pthread
    Pokretanje:
        sudo ./displej