#ifndef GPSDRIVER_HPP_INCLUDED
#define GPSDRIVER_HPP_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <bitset>

using namespace std;

char uartBuf[100];
const char *A1 = "GPGGA";
char GPGGA[100];
const char *A2 = "GPGSA";
char GPGSA[100];
const char *A3 = "GPRMC";
char GPRMC[100];
const char *A4 = "GPVTG";
char GPVTG[100];

#define spew (1)

struct gpsData
{
    int hHora;
    int hMin;
    int hSec;
    int year;
    int month;
    int day;
    double latitude;
    double longitude;
    int fixType;
    int numSat;
    float hdop;
    float altitude;
    float heading;
};

struct gpsData gpsInfo;

void getTime(char *time)
{
    int valor = atoi(time);
    gpsInfo.hHora = valor/10000;
    valor = valor%10000;
    gpsInfo.hMin = valor/100;
    valor = valor%100;
    gpsInfo.hSec = valor;
    #if spew == 1
    printf("\rTime GPS Fix  : %d:%d:%d UTC\n",gpsInfo.hHora,gpsInfo.hMin,gpsInfo.hSec);
    #endif // spew
}

void getLatitude(char *latitude, char *latSig)
{
    int tam = strlen(latitude);
    char latMin[7];
    int t = 0;
    while( t<7 )
    {
        latMin[t]=latitude[tam-(7-t)];
        t++;
    }
    latMin[7]='\0';
    double latMinutes = atof(latMin);
    int diff = tam - 7;
    char degLat[diff];
    t = 0;
    while( t < diff)
    {
        degLat[t] = latitude[t];
        t++;
    }
    degLat[t]='\0';
    double latDegress = atof(degLat);
    gpsInfo.latitude = latDegress + (latMinutes/60);
    if(latSig[0] == 'S')
        gpsInfo.latitude = -gpsInfo.latitude;
    #if spew == 1
    printf("\rLatitude      : %.10g\n",gpsInfo.latitude);
    #endif // spew

}

void getLongitude(char *longitude, char *lonSig)
{
    int tam = strlen(longitude);
    char lonMin[7];
    int t = 0;
    while( t<7 )
    {
        lonMin[t]=longitude[tam-(7-t)];
        t++;
    }
    lonMin[7]='\0';
    double lonMinutes = atof(lonMin);
    int diff = tam - 7;
    char degLon[diff];
    t = 0;
    while( t < diff)
    {
        degLon[t] = longitude[t];
        t++;
    }
    degLon[t]='\0';
    double lonDegress = atof(degLon);
    gpsInfo.longitude = lonDegress + (lonMinutes/60);
    if(lonSig[0] == 'W')
        gpsInfo.longitude = -gpsInfo.longitude;
    #if spew == 1
    printf("\rLongitude     : %.10g\n",gpsInfo.longitude);
    #endif // spew

}

int getFixType( char *fixType)
{
    int tam = strlen(fixType);
    fixType[tam]='\0';
    gpsInfo.fixType = atoi(fixType);
    #if spew == 1
    printf("\rGPS Fix Type  : %d\n",gpsInfo.fixType);
    #endif // spew
    return gpsInfo.fixType;
}

void getNumSat( char *numSat)
{
    int tam = strlen(numSat);
    numSat[tam]='\0';
    gpsInfo.numSat = atoi(numSat);
    #if spew == 1
    printf("\rNumber of sat : %d\n",gpsInfo.numSat);
    #endif // spew
}

void getHdop( char *hdop)
{
    int tam = strlen(hdop);
    hdop[tam]='\0';
    gpsInfo.hdop = atof(hdop);
    #if spew == 1
    printf("\rHdop          : %.2f\n",gpsInfo.hdop);
    #endif // spew
}

void getAltitude( char* altitude)
{
    int tam = strlen(altitude);
    altitude[tam]='\0';
    gpsInfo.altitude = atof(altitude);
    #if spew == 1
    printf("\rAltitude      : %.1f\n",gpsInfo.altitude);
    #endif // spew
}

bool infoGPGGA(char *GPGGA)
{
    char latitude[20];
    char longitude[20];
    char *word;
    int step=0,fixType=0;
    word = strtok(GPGGA, ",");
    while(word != NULL)
    {
        step++;
        word = strtok(NULL, ",");
        switch(step)
        {
            case 1:
                getTime(word);
                break;
            case 2:
                sprintf(latitude, "%s", word);
                word = strtok(NULL, ",");
                getLatitude(latitude, word);
                break;
            case 3:
                sprintf(longitude, "%s", word);
                word = strtok(NULL, ",");
                getLongitude(longitude, word);
                break;
            case 4:
                fixType=getFixType(word);
                break;
            case 5:
                if(fixType!=0)
                    getNumSat(word);
                break;
            case 6:
                getHdop(word);
                break;
            case 7:
                getAltitude(word);
                break;
            default:
                break;
        }
    }
    return true;
}

void getHeading(char *heading)
{
    int tam = strlen(heading);
    heading[tam]='\0';
    gpsInfo.heading = atof(heading);
    #if spew == 1
    printf("\rHeading       : %.1fº\n",gpsInfo.heading);
    #endif // spew
}

void getDate( char *date)
{
    int valor = atoi(date);
    gpsInfo.day = valor/10000;
    valor = valor%10000;
    gpsInfo.month = valor/100;
    valor = valor%100;
    gpsInfo.year = valor;
    #if spew == 1
    printf("\rDate          : %d-%d-%d\n",gpsInfo.day,gpsInfo.month,2000 + gpsInfo.year);
    #endif // spew
}

bool infoGPRMC(char *GPRMC)
{
    if(gpsInfo.fixType!=0)
    {
        char *word;
        int t = 1;
        int step=1;
        word = strtok(GPRMC, ",");
        while(word != NULL)
        {
            if(t < 8)
            {
                word = strtok(NULL, ",");
                t++;
            }
            else
            {
                word = strtok(NULL, ",");
                switch(step)
                {
                    case 1:
                        getHeading(word);
                        break;
                    case 2:
                        getDate(word);
                        break;
                    default:
                        break;
                }
                step++;
            }
        }
    }
    return true;
}

#endif // GPSDRIVER_HPP_INCLUDED
