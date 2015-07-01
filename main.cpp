#include "gpsDriver.hpp"

struct termios tio;
struct termios stdio;
struct termios old_stdio;
int tty_fd;

#define SERIAL_PORT         "/dev/ttyUSB0"

void inicSerial(void)
{
    tcgetattr(STDOUT_FILENO,&old_stdio);

    memset(&stdio,0,sizeof(stdio));
    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VMIN]=1;
    stdio.c_cc[VTIME]=0;
    tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
    tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    tty_fd=open(SERIAL_PORT, O_RDWR | O_NONBLOCK);
    cfsetospeed(&tio,B38400);            // 115200 baud
    cfsetispeed(&tio,B38400);            // 115200 baud

    tcsetattr(tty_fd,TCSANOW,&tio);

}

int main(int argc,char** argv)
{
    inicSerial();
    unsigned char c='D';

    GPGGA[0] = '\0';
    GPGSA[0] = '\0';
    GPRMC[0] = '\0';
    GPVTG[0] = '\0';
    int i=0;
    while(c!='$')
    {
        read(tty_fd,&c,1);
    }

    //!PRINT $XXXX
    int out_info = 0;
    while (c!='q')
    {
        if (read(tty_fd,&c,1)>0)
        {
            if(c!='$')
            {
                uartBuf[i]=c;
                i++;
            }
            else
            {
                //!$GPGGA - Global Positioning System Fix Data
                if(strstr(uartBuf,A1) != NULL)
                {
                    int tam = strlen(uartBuf);
                    uartBuf[tam-1]='\0';
                    sprintf(GPGGA,"%s",uartBuf);
                    infoGPGGA(GPGGA);
                    if(out_info == 1)
                        printf("%s\n",GPGGA);
                }
                //!$GPGSA - GPS DOP and active satellites
                if(strstr(uartBuf,A2) != NULL)
                {
                    int tam = strlen(uartBuf);
                    uartBuf[tam-1]='\0';
                    strcpy(GPGSA,uartBuf);
                    if(out_info == 2)
                        printf("%s\n",GPGSA);
                }
                //!$GPRMC - Recommended minimum specific GPS/Transit data
                if(strstr(uartBuf,A3) != NULL)
                {
                    int tam = strlen(uartBuf);
                    uartBuf[tam-1]='\0';
                    strcpy(GPRMC,uartBuf);
                    if(out_info == 3)
                        printf("%s\n",GPRMC);
                    infoGPRMC(GPRMC);
                    printf("\r\n\r");
                }
                //!$GPVTG - Track made good and ground speed
                if(strstr(uartBuf,A4) != NULL)
                {
                    int tam = strlen(uartBuf);
                    uartBuf[tam-1]='\0';
                    strcpy(GPVTG,uartBuf);
                    if(out_info == 4)
                        printf("%s\n",GPVTG);
                }
                i=0;
                while(i<100)
                {
                    uartBuf[i]='\0';
                    i++;
                }
                i=0;
            }
            //printf("%c",c);
            //write(STDOUT_FILENO,&c,1);              // if new data is available on the serial port, print it out
        }
        // if (read(STDIN_FILENO,&c,1)>0)  write(tty_fd,&c,1);                     // if new data is available on the console, send it to the serial port
        read(STDIN_FILENO,&c,1);
    }

    close(tty_fd);
    tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);

    return EXIT_SUCCESS;
}
