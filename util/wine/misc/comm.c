/*
 * DEC 93 Erik Bos (erik@trashcan.hacktic.nl)
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#if defined(__NetBSD__) || defined(__FreeBSD__)
#include <errno.h>
#include <sys/ioctl.h>
#endif
#include <unistd.h>

#include "wine.h"
#include "windows.h"

#define DEBUG_COMM

#define MAX_PORTS	16

int commerror = 0, eventmask = 0;

struct DosDeviceStruct {
	char *devicename;	/* /dev/cua1 */
	int fd;
	int suspended;
	int unget;
	int unget_byte;
};

struct DosDeviceStruct COM[MAX_PORTS];
struct DosDeviceStruct LPT[MAX_PORTS];

void Comm_DeInit(void);

void Comm_Init(void)
{
	int x, serial = 0, parallel = 0;
	char option[10], temp[256];
	struct stat st;

	for (x=0; x!=MAX_PORTS; x++) {
		strcpy(option,"COMx");
		option[3] = '0' + x;
		option[4] = '\0';

		GetPrivateProfileString("serialports", option, "*", temp, sizeof(temp), WINE_INI);
		if (!strcmp(temp, "*") || *temp == '\0') 
			COM[serial].devicename = NULL;
		else {
			stat(temp, &st);
			if (!S_ISCHR(st.st_mode)) 
				fprintf(stderr,"comm: can 't use `%s' as COM%d !\n", temp, x);
			else
				if ((COM[serial].devicename = malloc(strlen(temp)+1)) == NULL) 
					fprintf(stderr,"comm: can't malloc for device info!\n");
				else {
					COM[serial].fd = 0;
					strcpy(COM[serial].devicename, temp);
					serial++;
				}
		}

		strcpy(option, "LPTx");
		option[3] = '0' + x;
		option[4] = '\0';

		GetPrivateProfileString("parallelports", option, "*", temp, sizeof(temp), WINE_INI);
		if (!strcmp(temp, "*") || *temp == '\0')
			LPT[parallel].devicename = NULL;
		else {
			stat(temp, &st);
			if (!S_ISCHR(st.st_mode)) 
				fprintf(stderr,"comm: can 't use `%s' as LPT%d !\n", temp, x);
			else 
				if ((LPT[parallel].devicename = malloc(strlen(temp)+1)) == NULL) 
					fprintf(stderr,"comm: can't malloc for device info!\n");
				else {
					LPT[parallel].fd = 0;
					strcpy(LPT[parallel].devicename, temp);
					parallel++;
				}
		}

	}
}

void Comm_DeInit(void)
{
	int x;

	for (x=0; x!=MAX_PORTS; x++) {

		if (COM[x].devicename) {
			if (COM[x].fd)
	    			close(COM[x].fd);
	        	free(COM[x].devicename);
	    	}
		if (LPT[x].devicename) {
			if (LPT[x].fd)
		    		close(LPT[x].fd);
			free(LPT[x].devicename);
		}
	}
}

struct DosDeviceStruct *GetDeviceStruct(int fd)
{
	int x;
	
	for (x=0; x!=MAX_PORTS; x++) {
	    if (COM[x].fd == fd)
		return &COM[x];
	    if (LPT[x].fd == fd)
		return &LPT[x];
	}

	return NULL;
}

int ValidCOMPort(int x)
{
	return(x < MAX_PORTS ? (int) COM[x].devicename : 0); 
}

int ValidLPTPort(int x)
{
	return(x < MAX_PORTS ? (int) LPT[x].devicename : 0); 
}

int WinError(void)
{
	perror("comm");
	switch (errno) {
		default:
			return CE_IOE;
		}
}

int BuildCommDCB(LPSTR device, DCB FAR *lpdcb)
{
	/* "COM1:9600,n,8,1"	*/
	/*  012345		*/

	int port;
	char *ptr, temp[256];

#ifdef DEBUG_COMM
fprintf(stderr,"BuildCommDCB: (%s), ptr %d\n", device, (long) lpdcb);
#endif
	commerror = 0;

	if (!strncmp(device,"COM",3)) {
		port = device[3] - '0';
	

		if (port-- == 0) {
			fprintf(stderr, "comm: BUG ! COM0 can't exists!.\n");
			commerror = IE_BADID;
		}

		if (!ValidCOMPort(port)) {
			commerror = IE_BADID;
			return -1;
		}
		
		if (!COM[port].fd) {
			commerror = IE_NOPEN;
			return -1;
		}
		lpdcb->Id = COM[port].fd;
		
		if (!*(device+4))
			return 0;

		if (*(device+4) != ':')
			return -1;
		
		strcpy(temp,device+5);
		ptr = strtok(temp, ","); 

		fprintf(stderr,"BuildCommDCB: baudrate (%s)\n", ptr);
		lpdcb->BaudRate = atoi(ptr);

		ptr = strtok(NULL, ",");
		if (islower(*ptr))
			*ptr = toupper(*ptr);

		fprintf(stderr,"BuildCommDCB: parity (%c)\n", *ptr);
		switch (*ptr) {
			case 'N':
				lpdcb->Parity = NOPARITY;
				lpdcb->fParity = 0;
				break;			
			
			lpdcb->fParity = 1;
			
			case 'E':
				lpdcb->Parity = EVENPARITY;
				break;			
			case 'M':
				lpdcb->Parity = MARKPARITY;
				break;			
			case 'O':
				lpdcb->Parity = ODDPARITY;
				break;			
			default:
				fprintf(stderr,"comm: unknown parity `%c'!\n", *ptr);
				return -1;
		}

		ptr = strtok(NULL, ","); 
		fprintf(stderr, "BuildCommDCB: charsize (%c)\n", *ptr);
		lpdcb->ByteSize = *ptr - '0';

		ptr = strtok(NULL, ",");
		fprintf(stderr, "BuildCommDCB: stopbits (%c)\n", *ptr);
		switch (*ptr) {
			case '1':
				lpdcb->StopBits = ONESTOPBIT;
				break;			
			case '2':
				lpdcb->StopBits = TWOSTOPBITS;
				break;			
			default:
				fprintf(stderr,"comm: unknown # of stopbits `%c'!\n", *ptr);
				return -1;
		}
	}	

	return 0;
}

int OpenComm(LPSTR device, UINT cbInQueue, UINT cbOutQueue)
{
	int port, fd;

#ifdef DEBUG_COMM
fprintf(stderr,"OpenComm: %s, %d, %d\n", device, cbInQueue, cbOutQueue);
#endif	
	
	commerror = 0;

	if (!strncmp(device,"COM",3)) {
		port = device[3] - '0';

		if (port-- == 0) {
			fprintf(stderr, "comm: BUG ! COM0 doesn't exists!.\n");
			commerror = IE_BADID;
		}

		if (!ValidCOMPort(port)) {
			commerror = IE_BADID;
			return -1;
		}
		if (COM[port].fd) {
			commerror = IE_OPEN;
			return -1;
		}

		fd = open(COM[port].devicename, O_RDWR | O_NONBLOCK, 0);
		if (fd == -1) {
			commerror = WinError();
			return -1;
		} else {
			COM[port].fd = fd;	
			return fd;
		}
	} 
	else 
	if (!strncmp(device,"LPT",3)) {
		port = device[3] - '0';
	
		if (!ValidLPTPort(port)) {
			commerror = IE_BADID;
			return -1;
		}		
		if (LPT[port].fd) {
			commerror = IE_OPEN;
			return -1;
		}

		fd = open(LPT[port].devicename, O_RDWR | O_NONBLOCK, 0);
		if (fd == -1) {
			commerror = WinError();
			return -1;	
		} else {
			LPT[port].fd = fd;
			return fd;
		}
	}
	return 0;
}

int CloseComm(int fd)
{
#ifdef DEBUG_COMM
fprintf(stderr,"CloseComm: fd %d\n", fd);
#endif	

	if (close(fd) == -1) {
		commerror = WinError();
		return -1;
	} else {
		commerror = 0;
		return 0;
	}
}

int SetCommBreak(int fd)
{
	struct DosDeviceStruct *ptr;

#ifdef DEBUG_COMM
fprintf(stderr,"SetCommBreak: fd: %d\n", fd);
#endif	

	if ((ptr = GetDeviceStruct(fd)) == NULL) {
		commerror = IE_BADID;
		return -1;
	}

	ptr->suspended = 1;
	commerror = 0;
	return 0;
}

int ClearCommBreak(int fd)
{
	struct DosDeviceStruct *ptr;

#ifdef DEBUG_COMM
fprintf(stderr,"ClearCommBreak: fd: %d\n", fd);
#endif	

	if ((ptr = GetDeviceStruct(fd)) == NULL) {
		commerror = IE_BADID;
		return -1;
	}

	ptr->suspended = 0;
	commerror = 0;
	return 0;
}

LONG EscapeCommFunction(int fd, int nFunction)
{
	int max;
	struct termios port;

#ifdef DEBUG_COMM
fprintf(stderr,"EscapeCommFunction fd: %d, function: %d\n", fd, nFunction);
#endif

	if (tcgetattr(fd, &port) == -1) {
		commerror = WinError();	
		return -1;
	}

	switch (nFunction) {
		case RESETDEV:
			break;					

		case GETMAXCOM:
			for (max = 0;COM[max].devicename;max++)
				;		
			return max;
			break;

		case GETMAXLPT:
			for (max = 0;LPT[max].devicename;max++)
				;		
			return 0x80 + max;
			break;

		case CLRDTR:
			port.c_cflag &= TIOCM_DTR;
			break;

		case CLRRTS:
			port.c_cflag &= TIOCM_RTS;
			break;
	
		case SETDTR:
			port.c_cflag |= CRTSCTS;
			break;

		case SETRTS:
			port.c_cflag |= CRTSCTS;
			break;

		case SETXOFF:
			port.c_iflag |= IXOFF;
			break;

		case SETXON:
			port.c_iflag |= IXON;
			break;

		default:
			fprintf(stderr,"EscapeCommFunction fd: %d, unknown function: %d\n", fd, nFunction);
			break;				
	}
	
	if (tcsetattr(fd, TCSADRAIN, &port) == -1) {
		commerror = WinError();
		return -1;	
	} else {
		commerror = 0;
		return 0;
	}
}

int FlushComm(int fd, int fnQueue)
{
	int queue;

#ifdef DEBUG_COMM
fprintf(stderr,"FlushComm fd: %d, queue: %d\n", fd, fnQueue);
#endif	

	switch (fnQueue) {
		case 0:
			queue = TCOFLUSH;
			break;
		case 1:
			queue = TCIFLUSH;
			break;
		default:
			fprintf(stderr,"FlushComm fd: %d, UNKNOWN queue: %d\n", fd, fnQueue);
			return -1;
		}
	
	if (tcflush(fd, fnQueue)) {
		commerror = WinError();
		return -1;	
	} else {
		commerror = 0;
		return 0;
	}
}  

int GetCommError(int fd, COMSTAT FAR *lpStat)
{
#ifdef DEBUG_COMM
fprintf(stderr,"GetCommError: fd %d (current error %d)\n", fd, commerror);
#endif	

	return(commerror);
}

UINT FAR* SetCommEventMask(int fd, UINT fuEvtMask)
{
#ifdef DEBUG_COMM
fprintf(stderr,"SetCommEventMask: fd %d, mask %d\n", fd, fuEvtMask);
#endif	
	
	eventmask |= fuEvtMask;
	return (UINT *)&eventmask;
}

UINT GetCommEventMask(int fd, int fnEvtClear)
{
#ifdef DEBUG_COMM
fprintf(stderr,"GetCommEventMask: fd %d, mask %d\n", fd, fnEvtClear);
#endif	
	eventmask &= ~fnEvtClear;
	return eventmask;
}

int SetCommState(DCB FAR *lpdcb)
{
	struct termios port;

#ifdef DEBUG_COMM
fprintf(stderr,"SetCommState: fd %d, ptr %d\n", lpdcb->Id, (long) lpdcb);
#endif	

	if (tcgetattr(lpdcb->Id, &port) == -1) {
		commerror = WinError();	
		return -1;
	}

	port.c_cc[VMIN] = 0;
	port.c_cc[VTIME] = 1;

	port.c_iflag &= ~(ISTRIP|BRKINT|IGNCR|ICRNL|INLCR|IMAXBEL);
	port.c_iflag |= (IGNBRK);

	port.c_oflag &= ~(OPOST);

	port.c_cflag &= ~(HUPCL);
	port.c_cflag |= CLOCAL | CREAD;

	port.c_lflag &= ~(ICANON|ECHO|ISIG);
	port.c_lflag |= NOFLSH;

	fprintf(stderr,"SetCommState: baudrate %d\n",lpdcb->BaudRate);
#ifdef CBAUD
	port.c_cflag &= ~CBAUD;
	switch (lpdcb->BaudRate) {
		case 110:
		case CBR_110:
			port.c_cflag |= B110;
			break;		
		case 300:
		case CBR_300:
			port.c_cflag |= B300;
			break;		
		case 600:
		case CBR_600:
			port.c_cflag |= B600;
			break;		
		case 1200:
		case CBR_1200:
			port.c_cflag |= B1200;
			break;		
		case 2400:
		case CBR_2400:
			port.c_cflag |= B2400;
			break;		
		case 4800:
		case CBR_4800:
			port.c_cflag |= B4800;
			break;		
		case 9600:
		case CBR_9600:
			port.c_cflag |= B9600;
			break;		
		case 19200:
		case CBR_19200:
			port.c_cflag |= B19200;
			break;		
		case 38400:
		case CBR_38400:
			port.c_cflag |= B38400;
			break;		
		default:
			commerror = IE_BAUDRATE;
			return -1;
	}
#else
        switch (lpdcb->BaudRate) {
                case 110:
                case CBR_110:
                        port.c_ospeed = B110;
                        break;
                case 300:
                case CBR_300:
                        port.c_ospeed = B300;
                        break;
                case 600:
                case CBR_600:
                        port.c_ospeed = B600;
                        break;
                case 1200:
                case CBR_1200:
                        port.c_ospeed = B1200;
                        break;
                case 2400:
                case CBR_2400:
                        port.c_ospeed = B2400;
                        break;
                case 4800:
                case CBR_4800:
                        port.c_ospeed = B4800;
                        break;
                case 9600:
                case CBR_9600:
                        port.c_ospeed = B9600;
                        break;
                case 19200:
                case CBR_19200:
                        port.c_ospeed = B19200;
                        break;
                case 38400:
                case CBR_38400:
                        port.c_ospeed = B38400;
                        break;
                default:
                        commerror = IE_BAUDRATE;
                        return -1;
        }
        port.c_ispeed = port.c_ospeed;
#endif
	fprintf(stderr,"SetCommState: bytesize %d\n",lpdcb->ByteSize);
	port.c_cflag &= ~CSIZE;
	switch (lpdcb->ByteSize) {
		case 5:
			port.c_cflag |= CS5;
			break;
		case 6:
			port.c_cflag |= CS6;
			break;
		case 7:
			port.c_cflag |= CS7;
			break;
		case 8:
			port.c_cflag |= CS8;
			break;
		default:
			commerror = IE_BYTESIZE;
			return -1;
	}

	fprintf(stderr,"SetCommState: parity %d\n",lpdcb->Parity);
	port.c_cflag &= ~(PARENB | PARODD);
	if (lpdcb->fParity)
		switch (lpdcb->Parity) {
			case NOPARITY:
				port.c_iflag &= ~INPCK;
				break;
			case ODDPARITY:
				port.c_cflag |= (PARENB | PARODD);
				port.c_iflag |= INPCK;
				break;
			case EVENPARITY:
				port.c_cflag |= PARENB;
				port.c_iflag |= INPCK;
				break;
			default:
				commerror = IE_BYTESIZE;
				return -1;
		}
	

	fprintf(stderr,"SetCommState: stopbits %d\n",lpdcb->StopBits);
	switch (lpdcb->StopBits) {
		case ONESTOPBIT:
				port.c_cflag &= ~CSTOPB;
				break;
		case TWOSTOPBITS:
				port.c_cflag |= CSTOPB;
				break;
		default:
			commerror = IE_BYTESIZE;
			return -1;
	}

	if (lpdcb->fDtrflow || lpdcb->fRtsflow || lpdcb->fOutxCtsFlow)
		port.c_cflag |= CRTSCTS;

	if (lpdcb->fDtrDisable) 
		port.c_cflag &= ~CRTSCTS;
	
	if (lpdcb->fInX)
		port.c_iflag |= IXON;
	if (lpdcb->fOutX)
		port.c_iflag |= IXOFF;

	if (tcsetattr(lpdcb->Id, TCSADRAIN, &port) == -1) {
		commerror = WinError();	
		return -1;
	} else {
		commerror = 0;
		return 0;
	}
}

int GetCommState(int fd, DCB FAR *lpdcb)
{
	struct termios port;

#ifdef DEBUG_COMM
fprintf(stderr,"GetCommState: fd %d, ptr %d\n", fd, (long) lpdcb);
#endif

	if (tcgetattr(fd, &port) == -1) {
		commerror = WinError();	
		return -1;
	}

	lpdcb->Id = fd;

#ifdef CBAUD
        switch (port.c_cflag & CBAUD) {
#else
        switch (port.c_ospeed) {
#endif
		case B110:
			lpdcb->BaudRate = 110;
			break;
		case B300:
			lpdcb->BaudRate = 300;
			break;
		case B600:
			lpdcb->BaudRate = 600;
			break;
		case B1200:
			lpdcb->BaudRate = 1200;
			break;
		case B2400:
			lpdcb->BaudRate = 2400;
			break;
		case B4800:
			lpdcb->BaudRate = 4800;
			break;
		case B9600:
			lpdcb->BaudRate = 9600;
			break;
		case B19200:
			lpdcb->BaudRate = 19200;
			break;
		case B38400:
			lpdcb->BaudRate = 38400;
			break;
	}

	switch (port.c_cflag & CSIZE) {
		case CS5:
			lpdcb->ByteSize = 5;
			break;
		case CS6:
			lpdcb->ByteSize = 6;
			break;
		case CS7:
			lpdcb->ByteSize = 7;
			break;
		case CS8:
			lpdcb->ByteSize = 8;
			break;
	}	
	
	switch (port.c_cflag & ~(PARENB | PARODD)) {
		case 0:
			lpdcb->fParity = NOPARITY;
			break;
		case PARENB:
			lpdcb->fParity = EVENPARITY;
			break;
		case (PARENB | PARODD):
			lpdcb->fParity = ODDPARITY;		
			break;
	}

	if (port.c_cflag & CSTOPB)
		lpdcb->StopBits = TWOSTOPBITS;
	else
		lpdcb->StopBits = ONESTOPBIT;

	lpdcb->RlsTimeout = 50;
	lpdcb->CtsTimeout = 50; 
	lpdcb->DsrTimeout = 50;
	lpdcb->fNull = 0;
	lpdcb->fChEvt = 0;
	lpdcb->fBinary = 1;

	lpdcb->fDtrDisable = 0;
	if (port.c_cflag & CRTSCTS) {
		lpdcb->fDtrflow = 1;
		lpdcb->fRtsflow = 1;
		lpdcb->fOutxCtsFlow = 1;
		lpdcb->fOutxDsrFlow = 1;
	} else 
		lpdcb->fDtrDisable = 1;

	if (port.c_iflag & IXON)
		lpdcb->fInX = 1;
	else
		lpdcb->fInX = 0;

	if (port.c_iflag & IXOFF)
		lpdcb->fOutX = 1;
	else
		lpdcb->fOutX = 0;
/*
	lpdcb->XonChar = 
	lpdcb->XoffChar = 
 */
	lpdcb->XonLim = 10;
	lpdcb->XoffLim = 10;

	commerror = 0;
	return 0;
}

int TransmitCommChar(int fd, char chTransmit)
{
	struct DosDeviceStruct *ptr;

#ifdef DEBUG_COMM
fprintf(stderr,"TransmitCommChar: fd %d, data %d \n", fd, chTransmit);
#endif	

	if ((ptr = GetDeviceStruct(fd)) == NULL) {
		commerror = IE_BADID;
		return -1;
	}

	if (ptr->suspended) {
		commerror = IE_HARDWARE;
		return -1;
	}	

	if (write(fd, (void *) &chTransmit, 1) == -1) {
		commerror = WinError();
		return -1;	
	}  else {
		commerror = 0;
		return 0;
	}
}

int UngetCommChar(int fd, char chUnget)
{
	struct DosDeviceStruct *ptr;

#ifdef DEBUG_COMM
fprintf(stderr,"UngetCommChar: fd %d (char %d)\n", fd, chUnget);
#endif	

	if ((ptr = GetDeviceStruct(fd)) == NULL) {
		commerror = IE_BADID;
		return -1;
	}

	if (ptr->suspended) {
		commerror = IE_HARDWARE;
		return -1;
	}	

	ptr->unget = 1;
	ptr->unget_byte = chUnget;
	
	commerror = 0;
	return 0;
}

int ReadComm(int fd, LPSTR lpvBuf, int cbRead)
{
	int status, length;
	struct DosDeviceStruct *ptr;

#ifdef DEBUG_COMM
fprintf(stderr,"ReadComm: fd %d, ptr %d, length %d\n", fd, (long) lpvBuf, cbRead);
#endif	

	if ((ptr = GetDeviceStruct(fd)) == NULL) {
		commerror = IE_BADID;
		return -1;
	}

	if (ptr->suspended) {
		commerror = IE_HARDWARE;
		return -1;
	}	

	if (ptr->unget) {
		*lpvBuf = ptr->unget_byte;
		lpvBuf++;
		ptr->unget = 0;

		length = 1;
	} else
	 	length = 0;

	status = read(fd, (void *) lpvBuf, cbRead);

	if (status == -1) {
		commerror = WinError();
		return -1 - length;	
	} else {
		commerror = 0;
		return length + status;
	}
}

int WriteComm(int fd, LPSTR lpvBuf, int cbWrite)
{
	int x, length;
	struct DosDeviceStruct *ptr;

#ifdef DEBUG_COMM
fprintf(stderr,"WriteComm: fd %d, ptr %d, length %d\n", fd, (long) lpvBuf, cbWrite);
#endif
	
	if ((ptr = GetDeviceStruct(fd)) == NULL) {
		commerror = IE_BADID;
		return -1;
	}

	if (ptr->suspended) {
		commerror = IE_HARDWARE;
		return -1;
	}	
	
#ifdef DEBUG_COMM
	for (x=0; x != cbWrite ; x++)
		fprintf(stderr,"%c", *(lpvBuf + x) );
#endif

	length = write(fd, (void *) lpvBuf, cbWrite);
	
	if (length == -1) {
		commerror = WinError();
		return -1;	
	} else {
		commerror = 0;	
		return length;
	}
}
