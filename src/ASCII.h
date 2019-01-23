#ifndef _ASCII_H
#define _ASCII_H

#define NUL		0x00	// (null)	空字符	
#define SOH		0x01	// (start of headline)	标题开始	
#define STX		0x02	// (start of text)	正文开始	
#define ETX		0x03	// (end of text)	正文结束	
#define EOT		0x04	// (end of transmission)	传输结束	
#define ENQ		0x05	// (enquiry)	请求	
#define ACK		0x06	// (acknowledge)	收到通知	
#define BEL		0x07	// (bell)	响铃	
#define BS		0x08	// (backspace)	退格	
#define HT		0x09	// (horizontal tab)	水平制表符	
#define LF		0x0A	// (NL line feed, new line)	换行键	
#define VT		0x0B	// (vertical tab)	垂直制表符	
#define FF		0x0C	// (NP form feed, new page)	换页键	
#define CR		0x0D	// (carriage return)	回车键	
#define SO		0x0E	// (shift out)	不用切换	
#define SI		0x0F	// (shift in)	启用切换	
#define DLE		0x10	// (data link escape)	数据链路转义	
#define DC1		0x11	// (device control 1)	设备控制1	
#define DC2		0x12	// (device control 2)	设备控制2	
#define DC3		0x13	// (device control 3)	设备控制3	
#define DC4		0x14	// (device control 4)	设备控制4	
#define NAK		0x15	// (negative acknowledge)	拒绝接收	
#define SYN		0x16	// (synchronous idle)	同步空闲	
#define ETB		0x17	// (end of trans. block)	传输块结束	
#define CAN		0x18	// (cancel)	取消	
#define EM		0x19	// (end of medium)	介质中断	
#define SUB		0x1A	// (substitute)	替补	
#define ESC		0x1B	// (escape)	换码(溢出)	
#define FS		0x1C	// (file separator)	文件分割符	
#define GS		0x1D	// (group separator)	分组符	
#define RS		0x1E	// (record separator)	记录分离符	
#define US		0x1F	// (unit separator)	单元分隔符	
#define DEL		0x7F	// (delete)	删除	


#endif //_ASCII_H

