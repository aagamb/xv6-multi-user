
_init:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:

char *argv[] = { "sh", 0 };

int
main(void)
{
   0:	8d 4c 24 04          	lea    0x4(%esp),%ecx
   4:	83 e4 f0             	and    $0xfffffff0,%esp
   7:	ff 71 fc             	push   -0x4(%ecx)
   a:	55                   	push   %ebp
   b:	89 e5                	mov    %esp,%ebp
   d:	53                   	push   %ebx
   e:	51                   	push   %ecx
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
   f:	83 ec 08             	sub    $0x8,%esp
  12:	6a 02                	push   $0x2
  14:	68 a8 08 00 00       	push   $0x8a8
  19:	e8 51 04 00 00       	call   46f <open>
  1e:	83 c4 10             	add    $0x10,%esp
  21:	85 c0                	test   %eax,%eax
  23:	0f 88 9f 00 00 00    	js     c8 <main+0xc8>
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  29:	83 ec 0c             	sub    $0xc,%esp
  2c:	6a 00                	push   $0x0
  2e:	e8 74 04 00 00       	call   4a7 <dup>
  dup(0);  // stderr
  33:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  3a:	e8 68 04 00 00       	call   4a7 <dup>
  3f:	83 c4 10             	add    $0x10,%esp
  42:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

  for(;;){
    printf(1, "init: starting sh\n");
  48:	83 ec 08             	sub    $0x8,%esp
  4b:	68 b0 08 00 00       	push   $0x8b0
  50:	6a 01                	push   $0x1
  52:	e8 29 05 00 00       	call   580 <printf>
    pid = fork();
  57:	e8 cb 03 00 00       	call   427 <fork>
    if(pid < 0){
  5c:	83 c4 10             	add    $0x10,%esp
    pid = fork();
  5f:	89 c3                	mov    %eax,%ebx
    if(pid < 0){
  61:	85 c0                	test   %eax,%eax
  63:	78 2c                	js     91 <main+0x91>
      printf(1, "init: fork failed\n");
      exit();
    }
    if(pid == 0){
  65:	74 3d                	je     a4 <main+0xa4>
  67:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  6e:	66 90                	xchg   %ax,%ax
      exec("sh", argv);
      printf(1, "init: exec sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
  70:	e8 c2 03 00 00       	call   437 <wait>
  75:	85 c0                	test   %eax,%eax
  77:	78 cf                	js     48 <main+0x48>
  79:	39 c3                	cmp    %eax,%ebx
  7b:	74 cb                	je     48 <main+0x48>
      printf(1, "zombie!\n");
  7d:	83 ec 08             	sub    $0x8,%esp
  80:	68 ef 08 00 00       	push   $0x8ef
  85:	6a 01                	push   $0x1
  87:	e8 f4 04 00 00       	call   580 <printf>
  8c:	83 c4 10             	add    $0x10,%esp
  8f:	eb df                	jmp    70 <main+0x70>
      printf(1, "init: fork failed\n");
  91:	53                   	push   %ebx
  92:	53                   	push   %ebx
  93:	68 c3 08 00 00       	push   $0x8c3
  98:	6a 01                	push   $0x1
  9a:	e8 e1 04 00 00       	call   580 <printf>
      exit();
  9f:	e8 8b 03 00 00       	call   42f <exit>
      exec("sh", argv);
  a4:	50                   	push   %eax
  a5:	50                   	push   %eax
  a6:	68 6c 0c 00 00       	push   $0xc6c
  ab:	68 d6 08 00 00       	push   $0x8d6
  b0:	e8 b2 03 00 00       	call   467 <exec>
      printf(1, "init: exec sh failed\n");
  b5:	5a                   	pop    %edx
  b6:	59                   	pop    %ecx
  b7:	68 d9 08 00 00       	push   $0x8d9
  bc:	6a 01                	push   $0x1
  be:	e8 bd 04 00 00       	call   580 <printf>
      exit();
  c3:	e8 67 03 00 00       	call   42f <exit>
    mknod("console", 1, 1);
  c8:	50                   	push   %eax
  c9:	6a 01                	push   $0x1
  cb:	6a 01                	push   $0x1
  cd:	68 a8 08 00 00       	push   $0x8a8
  d2:	e8 a0 03 00 00       	call   477 <mknod>
    open("console", O_RDWR);
  d7:	58                   	pop    %eax
  d8:	5a                   	pop    %edx
  d9:	6a 02                	push   $0x2
  db:	68 a8 08 00 00       	push   $0x8a8
  e0:	e8 8a 03 00 00       	call   46f <open>
  e5:	83 c4 10             	add    $0x10,%esp
  e8:	e9 3c ff ff ff       	jmp    29 <main+0x29>
  ed:	66 90                	xchg   %ax,%ax
  ef:	90                   	nop

000000f0 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
  f0:	55                   	push   %ebp
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
  f1:	31 c0                	xor    %eax,%eax
{
  f3:	89 e5                	mov    %esp,%ebp
  f5:	53                   	push   %ebx
  f6:	8b 4d 08             	mov    0x8(%ebp),%ecx
  f9:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  while((*s++ = *t++) != 0)
 100:	0f b6 14 03          	movzbl (%ebx,%eax,1),%edx
 104:	88 14 01             	mov    %dl,(%ecx,%eax,1)
 107:	83 c0 01             	add    $0x1,%eax
 10a:	84 d2                	test   %dl,%dl
 10c:	75 f2                	jne    100 <strcpy+0x10>
    ;
  return os;
}
 10e:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 111:	89 c8                	mov    %ecx,%eax
 113:	c9                   	leave  
 114:	c3                   	ret    
 115:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 11c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000120 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 120:	55                   	push   %ebp
 121:	89 e5                	mov    %esp,%ebp
 123:	53                   	push   %ebx
 124:	8b 55 08             	mov    0x8(%ebp),%edx
 127:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
 12a:	0f b6 02             	movzbl (%edx),%eax
 12d:	84 c0                	test   %al,%al
 12f:	75 17                	jne    148 <strcmp+0x28>
 131:	eb 3a                	jmp    16d <strcmp+0x4d>
 133:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 137:	90                   	nop
 138:	0f b6 42 01          	movzbl 0x1(%edx),%eax
    p++, q++;
 13c:	83 c2 01             	add    $0x1,%edx
 13f:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
 142:	84 c0                	test   %al,%al
 144:	74 1a                	je     160 <strcmp+0x40>
    p++, q++;
 146:	89 d9                	mov    %ebx,%ecx
  while(*p && *p == *q)
 148:	0f b6 19             	movzbl (%ecx),%ebx
 14b:	38 c3                	cmp    %al,%bl
 14d:	74 e9                	je     138 <strcmp+0x18>
  return (uchar)*p - (uchar)*q;
 14f:	29 d8                	sub    %ebx,%eax
}
 151:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 154:	c9                   	leave  
 155:	c3                   	ret    
 156:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 15d:	8d 76 00             	lea    0x0(%esi),%esi
  return (uchar)*p - (uchar)*q;
 160:	0f b6 59 01          	movzbl 0x1(%ecx),%ebx
 164:	31 c0                	xor    %eax,%eax
 166:	29 d8                	sub    %ebx,%eax
}
 168:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 16b:	c9                   	leave  
 16c:	c3                   	ret    
  return (uchar)*p - (uchar)*q;
 16d:	0f b6 19             	movzbl (%ecx),%ebx
 170:	31 c0                	xor    %eax,%eax
 172:	eb db                	jmp    14f <strcmp+0x2f>
 174:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 17b:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 17f:	90                   	nop

00000180 <strlen>:

uint
strlen(const char *s)
{
 180:	55                   	push   %ebp
 181:	89 e5                	mov    %esp,%ebp
 183:	8b 55 08             	mov    0x8(%ebp),%edx
  int n;

  for(n = 0; s[n]; n++)
 186:	80 3a 00             	cmpb   $0x0,(%edx)
 189:	74 15                	je     1a0 <strlen+0x20>
 18b:	31 c0                	xor    %eax,%eax
 18d:	8d 76 00             	lea    0x0(%esi),%esi
 190:	83 c0 01             	add    $0x1,%eax
 193:	80 3c 02 00          	cmpb   $0x0,(%edx,%eax,1)
 197:	89 c1                	mov    %eax,%ecx
 199:	75 f5                	jne    190 <strlen+0x10>
    ;
  return n;
}
 19b:	89 c8                	mov    %ecx,%eax
 19d:	5d                   	pop    %ebp
 19e:	c3                   	ret    
 19f:	90                   	nop
  for(n = 0; s[n]; n++)
 1a0:	31 c9                	xor    %ecx,%ecx
}
 1a2:	5d                   	pop    %ebp
 1a3:	89 c8                	mov    %ecx,%eax
 1a5:	c3                   	ret    
 1a6:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 1ad:	8d 76 00             	lea    0x0(%esi),%esi

000001b0 <memset>:

void*
memset(void *dst, int c, uint n)
{
 1b0:	55                   	push   %ebp
 1b1:	89 e5                	mov    %esp,%ebp
 1b3:	57                   	push   %edi
 1b4:	8b 55 08             	mov    0x8(%ebp),%edx
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
 1b7:	8b 4d 10             	mov    0x10(%ebp),%ecx
 1ba:	8b 45 0c             	mov    0xc(%ebp),%eax
 1bd:	89 d7                	mov    %edx,%edi
 1bf:	fc                   	cld    
 1c0:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
 1c2:	8b 7d fc             	mov    -0x4(%ebp),%edi
 1c5:	89 d0                	mov    %edx,%eax
 1c7:	c9                   	leave  
 1c8:	c3                   	ret    
 1c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000001d0 <strchr>:

char*
strchr(const char *s, char c)
{
 1d0:	55                   	push   %ebp
 1d1:	89 e5                	mov    %esp,%ebp
 1d3:	8b 45 08             	mov    0x8(%ebp),%eax
 1d6:	0f b6 4d 0c          	movzbl 0xc(%ebp),%ecx
  for(; *s; s++)
 1da:	0f b6 10             	movzbl (%eax),%edx
 1dd:	84 d2                	test   %dl,%dl
 1df:	75 12                	jne    1f3 <strchr+0x23>
 1e1:	eb 1d                	jmp    200 <strchr+0x30>
 1e3:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 1e7:	90                   	nop
 1e8:	0f b6 50 01          	movzbl 0x1(%eax),%edx
 1ec:	83 c0 01             	add    $0x1,%eax
 1ef:	84 d2                	test   %dl,%dl
 1f1:	74 0d                	je     200 <strchr+0x30>
    if(*s == c)
 1f3:	38 d1                	cmp    %dl,%cl
 1f5:	75 f1                	jne    1e8 <strchr+0x18>
      return (char*)s;
  return 0;
}
 1f7:	5d                   	pop    %ebp
 1f8:	c3                   	ret    
 1f9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  return 0;
 200:	31 c0                	xor    %eax,%eax
}
 202:	5d                   	pop    %ebp
 203:	c3                   	ret    
 204:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 20b:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 20f:	90                   	nop

00000210 <gets>:

char*
gets(char *buf, int max)
{
 210:	55                   	push   %ebp
 211:	89 e5                	mov    %esp,%ebp
 213:	57                   	push   %edi
 214:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
 215:	8d 7d e7             	lea    -0x19(%ebp),%edi
{
 218:	53                   	push   %ebx
  for(i=0; i+1 < max; ){
 219:	31 db                	xor    %ebx,%ebx
{
 21b:	83 ec 1c             	sub    $0x1c,%esp
  for(i=0; i+1 < max; ){
 21e:	eb 27                	jmp    247 <gets+0x37>
    cc = read(0, &c, 1);
 220:	83 ec 04             	sub    $0x4,%esp
 223:	6a 01                	push   $0x1
 225:	57                   	push   %edi
 226:	6a 00                	push   $0x0
 228:	e8 1a 02 00 00       	call   447 <read>
    if(cc < 1)
 22d:	83 c4 10             	add    $0x10,%esp
 230:	85 c0                	test   %eax,%eax
 232:	7e 1d                	jle    251 <gets+0x41>
      break;
    buf[i++] = c;
 234:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
 238:	8b 55 08             	mov    0x8(%ebp),%edx
 23b:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 23f:	3c 0a                	cmp    $0xa,%al
 241:	74 1d                	je     260 <gets+0x50>
 243:	3c 0d                	cmp    $0xd,%al
 245:	74 19                	je     260 <gets+0x50>
  for(i=0; i+1 < max; ){
 247:	89 de                	mov    %ebx,%esi
 249:	83 c3 01             	add    $0x1,%ebx
 24c:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 24f:	7c cf                	jl     220 <gets+0x10>
      break;
  }
  buf[i] = '\0';
 251:	8b 45 08             	mov    0x8(%ebp),%eax
 254:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 258:	8d 65 f4             	lea    -0xc(%ebp),%esp
 25b:	5b                   	pop    %ebx
 25c:	5e                   	pop    %esi
 25d:	5f                   	pop    %edi
 25e:	5d                   	pop    %ebp
 25f:	c3                   	ret    
  buf[i] = '\0';
 260:	8b 45 08             	mov    0x8(%ebp),%eax
 263:	89 de                	mov    %ebx,%esi
 265:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
}
 269:	8d 65 f4             	lea    -0xc(%ebp),%esp
 26c:	5b                   	pop    %ebx
 26d:	5e                   	pop    %esi
 26e:	5f                   	pop    %edi
 26f:	5d                   	pop    %ebp
 270:	c3                   	ret    
 271:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 278:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 27f:	90                   	nop

00000280 <stat>:

int
stat(const char *n, struct stat *st)
{
 280:	55                   	push   %ebp
 281:	89 e5                	mov    %esp,%ebp
 283:	56                   	push   %esi
 284:	53                   	push   %ebx
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 285:	83 ec 08             	sub    $0x8,%esp
 288:	6a 00                	push   $0x0
 28a:	ff 75 08             	push   0x8(%ebp)
 28d:	e8 dd 01 00 00       	call   46f <open>
  if(fd < 0)
 292:	83 c4 10             	add    $0x10,%esp
 295:	85 c0                	test   %eax,%eax
 297:	78 27                	js     2c0 <stat+0x40>
    return -1;
  r = fstat(fd, st);
 299:	83 ec 08             	sub    $0x8,%esp
 29c:	ff 75 0c             	push   0xc(%ebp)
 29f:	89 c3                	mov    %eax,%ebx
 2a1:	50                   	push   %eax
 2a2:	e8 e0 01 00 00       	call   487 <fstat>
  close(fd);
 2a7:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
 2aa:	89 c6                	mov    %eax,%esi
  close(fd);
 2ac:	e8 a6 01 00 00       	call   457 <close>
  return r;
 2b1:	83 c4 10             	add    $0x10,%esp
}
 2b4:	8d 65 f8             	lea    -0x8(%ebp),%esp
 2b7:	89 f0                	mov    %esi,%eax
 2b9:	5b                   	pop    %ebx
 2ba:	5e                   	pop    %esi
 2bb:	5d                   	pop    %ebp
 2bc:	c3                   	ret    
 2bd:	8d 76 00             	lea    0x0(%esi),%esi
    return -1;
 2c0:	be ff ff ff ff       	mov    $0xffffffff,%esi
 2c5:	eb ed                	jmp    2b4 <stat+0x34>
 2c7:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 2ce:	66 90                	xchg   %ax,%ax

000002d0 <atoi>:

int
atoi(const char *s)
{
 2d0:	55                   	push   %ebp
 2d1:	89 e5                	mov    %esp,%ebp
 2d3:	53                   	push   %ebx
 2d4:	8b 55 08             	mov    0x8(%ebp),%edx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 2d7:	0f be 02             	movsbl (%edx),%eax
 2da:	8d 48 d0             	lea    -0x30(%eax),%ecx
 2dd:	80 f9 09             	cmp    $0x9,%cl
  n = 0;
 2e0:	b9 00 00 00 00       	mov    $0x0,%ecx
  while('0' <= *s && *s <= '9')
 2e5:	77 1e                	ja     305 <atoi+0x35>
 2e7:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 2ee:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
 2f0:	83 c2 01             	add    $0x1,%edx
 2f3:	8d 0c 89             	lea    (%ecx,%ecx,4),%ecx
 2f6:	8d 4c 48 d0          	lea    -0x30(%eax,%ecx,2),%ecx
  while('0' <= *s && *s <= '9')
 2fa:	0f be 02             	movsbl (%edx),%eax
 2fd:	8d 58 d0             	lea    -0x30(%eax),%ebx
 300:	80 fb 09             	cmp    $0x9,%bl
 303:	76 eb                	jbe    2f0 <atoi+0x20>
  return n;
}
 305:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 308:	89 c8                	mov    %ecx,%eax
 30a:	c9                   	leave  
 30b:	c3                   	ret    
 30c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000310 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 310:	55                   	push   %ebp
 311:	89 e5                	mov    %esp,%ebp
 313:	57                   	push   %edi
 314:	8b 45 10             	mov    0x10(%ebp),%eax
 317:	8b 55 08             	mov    0x8(%ebp),%edx
 31a:	56                   	push   %esi
 31b:	8b 75 0c             	mov    0xc(%ebp),%esi
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 31e:	85 c0                	test   %eax,%eax
 320:	7e 13                	jle    335 <memmove+0x25>
 322:	01 d0                	add    %edx,%eax
  dst = vdst;
 324:	89 d7                	mov    %edx,%edi
 326:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 32d:	8d 76 00             	lea    0x0(%esi),%esi
    *dst++ = *src++;
 330:	a4                   	movsb  %ds:(%esi),%es:(%edi)
  while(n-- > 0)
 331:	39 f8                	cmp    %edi,%eax
 333:	75 fb                	jne    330 <memmove+0x20>
  return vdst;
}
 335:	5e                   	pop    %esi
 336:	89 d0                	mov    %edx,%eax
 338:	5f                   	pop    %edi
 339:	5d                   	pop    %ebp
 33a:	c3                   	ret    
 33b:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 33f:	90                   	nop

00000340 <strcat>:

char* strcat(char* dest, const char* source)
{
 340:	55                   	push   %ebp
 341:	89 e5                	mov    %esp,%ebp
 343:	53                   	push   %ebx
 344:	8b 5d 08             	mov    0x8(%ebp),%ebx
 347:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  for(n = 0; s[n]; n++)
 34a:	80 3b 00             	cmpb   $0x0,(%ebx)
 34d:	74 41                	je     390 <strcat+0x50>
 34f:	8d 53 01             	lea    0x1(%ebx),%edx
 352:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 358:	89 d0                	mov    %edx,%eax
 35a:	83 c2 01             	add    $0x1,%edx
 35d:	80 7a ff 00          	cmpb   $0x0,-0x1(%edx)
 361:	75 f5                	jne    358 <strcat+0x18>
	char* ptr = dest + strlen(dest);
	while (*source != '\0')
 363:	0f b6 11             	movzbl (%ecx),%edx
 366:	84 d2                	test   %dl,%dl
 368:	74 15                	je     37f <strcat+0x3f>
 36a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
		*ptr++ = *source++;
 370:	88 10                	mov    %dl,(%eax)
 372:	83 c0 01             	add    $0x1,%eax
 375:	83 c1 01             	add    $0x1,%ecx
	while (*source != '\0')
 378:	0f b6 11             	movzbl (%ecx),%edx
 37b:	84 d2                	test   %dl,%dl
 37d:	75 f1                	jne    370 <strcat+0x30>
	*ptr = '\0';
 37f:	c6 00 00             	movb   $0x0,(%eax)
	return dest;
}
 382:	89 d8                	mov    %ebx,%eax
 384:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 387:	c9                   	leave  
 388:	c3                   	ret    
 389:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  for(n = 0; s[n]; n++)
 390:	89 d8                	mov    %ebx,%eax
	while (*source != '\0')
 392:	eb e4                	jmp    378 <strcat+0x38>
 394:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 39b:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 39f:	90                   	nop

000003a0 <strtok>:


char *strtok(char *str, const char *delim) {
 3a0:	55                   	push   %ebp
 3a1:	89 e5                	mov    %esp,%ebp
 3a3:	57                   	push   %edi
    static char *last_token = NULL;
    char *token;
    int delim_found = 0;

    if (str != NULL) {
 3a4:	8b 45 08             	mov    0x8(%ebp),%eax
char *strtok(char *str, const char *delim) {
 3a7:	8b 7d 0c             	mov    0xc(%ebp),%edi
 3aa:	56                   	push   %esi
 3ab:	53                   	push   %ebx
    if (str != NULL) {
 3ac:	85 c0                	test   %eax,%eax
 3ae:	74 6d                	je     41d <strtok+0x7d>
        last_token = str;
 3b0:	8b 45 08             	mov    0x8(%ebp),%eax
 3b3:	a3 74 0c 00 00       	mov    %eax,0xc74
    }

    while (*last_token != '\0') {
 3b8:	8b 45 08             	mov    0x8(%ebp),%eax
 3bb:	0f b6 08             	movzbl (%eax),%ecx
 3be:	84 c9                	test   %cl,%cl
 3c0:	74 54                	je     416 <strtok+0x76>
        const char *d = delim;
        while (*d != '\0') {
 3c2:	0f b6 37             	movzbl (%edi),%esi
 3c5:	8d 76 00             	lea    0x0(%esi),%esi
 3c8:	89 f0                	mov    %esi,%eax
 3ca:	89 fa                	mov    %edi,%edx
 3cc:	84 c0                	test   %al,%al
 3ce:	75 13                	jne    3e3 <strtok+0x43>
 3d0:	eb 2e                	jmp    400 <strtok+0x60>
 3d2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 3d8:	0f b6 42 01          	movzbl 0x1(%edx),%eax
            if (*last_token == *d) {
                delim_found = 1;
                break;
            }
            d++;
 3dc:	83 c2 01             	add    $0x1,%edx
        while (*d != '\0') {
 3df:	84 c0                	test   %al,%al
 3e1:	74 1d                	je     400 <strtok+0x60>
            if (*last_token == *d) {
 3e3:	38 c8                	cmp    %cl,%al
 3e5:	75 f1                	jne    3d8 <strtok+0x38>
        }
        if (delim_found) {
            *last_token = '\0';
 3e7:	8b 45 08             	mov    0x8(%ebp),%eax
 3ea:	c6 00 00             	movb   $0x0,(%eax)
            token = last_token - (strlen(last_token) - 1);
 3ed:	83 c0 01             	add    $0x1,%eax
        }
        last_token++;
    }

    return NULL;
}
 3f0:	5b                   	pop    %ebx
 3f1:	5e                   	pop    %esi
 3f2:	5f                   	pop    %edi
            last_token++;
 3f3:	a3 74 0c 00 00       	mov    %eax,0xc74
}
 3f8:	5d                   	pop    %ebp
 3f9:	c3                   	ret    
 3fa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        last_token++;
 400:	83 45 08 01          	addl   $0x1,0x8(%ebp)
    while (*last_token != '\0') {
 404:	8b 45 08             	mov    0x8(%ebp),%eax
 407:	0f b6 08             	movzbl (%eax),%ecx
 40a:	84 c9                	test   %cl,%cl
 40c:	75 ba                	jne    3c8 <strtok+0x28>
 40e:	8b 45 08             	mov    0x8(%ebp),%eax
 411:	a3 74 0c 00 00       	mov    %eax,0xc74
}
 416:	5b                   	pop    %ebx
    return NULL;
 417:	31 c0                	xor    %eax,%eax
}
 419:	5e                   	pop    %esi
 41a:	5f                   	pop    %edi
 41b:	5d                   	pop    %ebp
 41c:	c3                   	ret    
    while (*last_token != '\0') {
 41d:	a1 74 0c 00 00       	mov    0xc74,%eax
 422:	89 45 08             	mov    %eax,0x8(%ebp)
 425:	eb 91                	jmp    3b8 <strtok+0x18>

00000427 <fork>:
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
 427:	b8 01 00 00 00       	mov    $0x1,%eax
 42c:	cd 40                	int    $0x40
 42e:	c3                   	ret    

0000042f <exit>:
SYSCALL(exit)
 42f:	b8 02 00 00 00       	mov    $0x2,%eax
 434:	cd 40                	int    $0x40
 436:	c3                   	ret    

00000437 <wait>:
SYSCALL(wait)
 437:	b8 03 00 00 00       	mov    $0x3,%eax
 43c:	cd 40                	int    $0x40
 43e:	c3                   	ret    

0000043f <pipe>:
SYSCALL(pipe)
 43f:	b8 04 00 00 00       	mov    $0x4,%eax
 444:	cd 40                	int    $0x40
 446:	c3                   	ret    

00000447 <read>:
SYSCALL(read)
 447:	b8 05 00 00 00       	mov    $0x5,%eax
 44c:	cd 40                	int    $0x40
 44e:	c3                   	ret    

0000044f <write>:
SYSCALL(write)
 44f:	b8 10 00 00 00       	mov    $0x10,%eax
 454:	cd 40                	int    $0x40
 456:	c3                   	ret    

00000457 <close>:
SYSCALL(close)
 457:	b8 15 00 00 00       	mov    $0x15,%eax
 45c:	cd 40                	int    $0x40
 45e:	c3                   	ret    

0000045f <kill>:
SYSCALL(kill)
 45f:	b8 06 00 00 00       	mov    $0x6,%eax
 464:	cd 40                	int    $0x40
 466:	c3                   	ret    

00000467 <exec>:
SYSCALL(exec)
 467:	b8 07 00 00 00       	mov    $0x7,%eax
 46c:	cd 40                	int    $0x40
 46e:	c3                   	ret    

0000046f <open>:
SYSCALL(open)
 46f:	b8 0f 00 00 00       	mov    $0xf,%eax
 474:	cd 40                	int    $0x40
 476:	c3                   	ret    

00000477 <mknod>:
SYSCALL(mknod)
 477:	b8 11 00 00 00       	mov    $0x11,%eax
 47c:	cd 40                	int    $0x40
 47e:	c3                   	ret    

0000047f <unlink>:
SYSCALL(unlink)
 47f:	b8 12 00 00 00       	mov    $0x12,%eax
 484:	cd 40                	int    $0x40
 486:	c3                   	ret    

00000487 <fstat>:
SYSCALL(fstat)
 487:	b8 08 00 00 00       	mov    $0x8,%eax
 48c:	cd 40                	int    $0x40
 48e:	c3                   	ret    

0000048f <link>:
SYSCALL(link)
 48f:	b8 13 00 00 00       	mov    $0x13,%eax
 494:	cd 40                	int    $0x40
 496:	c3                   	ret    

00000497 <mkdir>:
SYSCALL(mkdir)
 497:	b8 14 00 00 00       	mov    $0x14,%eax
 49c:	cd 40                	int    $0x40
 49e:	c3                   	ret    

0000049f <chdir>:
SYSCALL(chdir)
 49f:	b8 09 00 00 00       	mov    $0x9,%eax
 4a4:	cd 40                	int    $0x40
 4a6:	c3                   	ret    

000004a7 <dup>:
SYSCALL(dup)
 4a7:	b8 0a 00 00 00       	mov    $0xa,%eax
 4ac:	cd 40                	int    $0x40
 4ae:	c3                   	ret    

000004af <getpid>:
SYSCALL(getpid)
 4af:	b8 0b 00 00 00       	mov    $0xb,%eax
 4b4:	cd 40                	int    $0x40
 4b6:	c3                   	ret    

000004b7 <sbrk>:
SYSCALL(sbrk)
 4b7:	b8 0c 00 00 00       	mov    $0xc,%eax
 4bc:	cd 40                	int    $0x40
 4be:	c3                   	ret    

000004bf <sleep>:
SYSCALL(sleep)
 4bf:	b8 0d 00 00 00       	mov    $0xd,%eax
 4c4:	cd 40                	int    $0x40
 4c6:	c3                   	ret    

000004c7 <uptime>:
SYSCALL(uptime)
 4c7:	b8 0e 00 00 00       	mov    $0xe,%eax
 4cc:	cd 40                	int    $0x40
 4ce:	c3                   	ret    
 4cf:	90                   	nop

000004d0 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 4d0:	55                   	push   %ebp
 4d1:	89 e5                	mov    %esp,%ebp
 4d3:	57                   	push   %edi
 4d4:	56                   	push   %esi
 4d5:	53                   	push   %ebx
 4d6:	83 ec 3c             	sub    $0x3c,%esp
 4d9:	89 4d c4             	mov    %ecx,-0x3c(%ebp)
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
 4dc:	89 d1                	mov    %edx,%ecx
{
 4de:	89 45 b8             	mov    %eax,-0x48(%ebp)
  if(sgn && xx < 0){
 4e1:	85 d2                	test   %edx,%edx
 4e3:	0f 89 7f 00 00 00    	jns    568 <printint+0x98>
 4e9:	f6 45 08 01          	testb  $0x1,0x8(%ebp)
 4ed:	74 79                	je     568 <printint+0x98>
    neg = 1;
 4ef:	c7 45 bc 01 00 00 00 	movl   $0x1,-0x44(%ebp)
    x = -xx;
 4f6:	f7 d9                	neg    %ecx
  } else {
    x = xx;
  }

  i = 0;
 4f8:	31 db                	xor    %ebx,%ebx
 4fa:	8d 75 d7             	lea    -0x29(%ebp),%esi
 4fd:	8d 76 00             	lea    0x0(%esi),%esi
  do{
    buf[i++] = digits[x % base];
 500:	89 c8                	mov    %ecx,%eax
 502:	31 d2                	xor    %edx,%edx
 504:	89 cf                	mov    %ecx,%edi
 506:	f7 75 c4             	divl   -0x3c(%ebp)
 509:	0f b6 92 58 09 00 00 	movzbl 0x958(%edx),%edx
 510:	89 45 c0             	mov    %eax,-0x40(%ebp)
 513:	89 d8                	mov    %ebx,%eax
 515:	8d 5b 01             	lea    0x1(%ebx),%ebx
  }while((x /= base) != 0);
 518:	8b 4d c0             	mov    -0x40(%ebp),%ecx
    buf[i++] = digits[x % base];
 51b:	88 14 1e             	mov    %dl,(%esi,%ebx,1)
  }while((x /= base) != 0);
 51e:	39 7d c4             	cmp    %edi,-0x3c(%ebp)
 521:	76 dd                	jbe    500 <printint+0x30>
  if(neg)
 523:	8b 4d bc             	mov    -0x44(%ebp),%ecx
 526:	85 c9                	test   %ecx,%ecx
 528:	74 0c                	je     536 <printint+0x66>
    buf[i++] = '-';
 52a:	c6 44 1d d8 2d       	movb   $0x2d,-0x28(%ebp,%ebx,1)
    buf[i++] = digits[x % base];
 52f:	89 d8                	mov    %ebx,%eax
    buf[i++] = '-';
 531:	ba 2d 00 00 00       	mov    $0x2d,%edx

  while(--i >= 0)
 536:	8b 7d b8             	mov    -0x48(%ebp),%edi
 539:	8d 5c 05 d7          	lea    -0x29(%ebp,%eax,1),%ebx
 53d:	eb 07                	jmp    546 <printint+0x76>
 53f:	90                   	nop
    putc(fd, buf[i]);
 540:	0f b6 13             	movzbl (%ebx),%edx
 543:	83 eb 01             	sub    $0x1,%ebx
  write(fd, &c, 1);
 546:	83 ec 04             	sub    $0x4,%esp
 549:	88 55 d7             	mov    %dl,-0x29(%ebp)
 54c:	6a 01                	push   $0x1
 54e:	56                   	push   %esi
 54f:	57                   	push   %edi
 550:	e8 fa fe ff ff       	call   44f <write>
  while(--i >= 0)
 555:	83 c4 10             	add    $0x10,%esp
 558:	39 de                	cmp    %ebx,%esi
 55a:	75 e4                	jne    540 <printint+0x70>
}
 55c:	8d 65 f4             	lea    -0xc(%ebp),%esp
 55f:	5b                   	pop    %ebx
 560:	5e                   	pop    %esi
 561:	5f                   	pop    %edi
 562:	5d                   	pop    %ebp
 563:	c3                   	ret    
 564:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  neg = 0;
 568:	c7 45 bc 00 00 00 00 	movl   $0x0,-0x44(%ebp)
 56f:	eb 87                	jmp    4f8 <printint+0x28>
 571:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 578:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 57f:	90                   	nop

00000580 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
 580:	55                   	push   %ebp
 581:	89 e5                	mov    %esp,%ebp
 583:	57                   	push   %edi
 584:	56                   	push   %esi
 585:	53                   	push   %ebx
 586:	83 ec 2c             	sub    $0x2c,%esp
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 589:	8b 5d 0c             	mov    0xc(%ebp),%ebx
{
 58c:	8b 75 08             	mov    0x8(%ebp),%esi
  for(i = 0; fmt[i]; i++){
 58f:	0f b6 13             	movzbl (%ebx),%edx
 592:	84 d2                	test   %dl,%dl
 594:	74 6a                	je     600 <printf+0x80>
  ap = (uint*)(void*)&fmt + 1;
 596:	8d 45 10             	lea    0x10(%ebp),%eax
 599:	83 c3 01             	add    $0x1,%ebx
  write(fd, &c, 1);
 59c:	8d 7d e7             	lea    -0x19(%ebp),%edi
  state = 0;
 59f:	31 c9                	xor    %ecx,%ecx
  ap = (uint*)(void*)&fmt + 1;
 5a1:	89 45 d0             	mov    %eax,-0x30(%ebp)
 5a4:	eb 36                	jmp    5dc <printf+0x5c>
 5a6:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 5ad:	8d 76 00             	lea    0x0(%esi),%esi
 5b0:	89 4d d4             	mov    %ecx,-0x2c(%ebp)
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
 5b3:	b9 25 00 00 00       	mov    $0x25,%ecx
      if(c == '%'){
 5b8:	83 f8 25             	cmp    $0x25,%eax
 5bb:	74 15                	je     5d2 <printf+0x52>
  write(fd, &c, 1);
 5bd:	83 ec 04             	sub    $0x4,%esp
 5c0:	88 55 e7             	mov    %dl,-0x19(%ebp)
 5c3:	6a 01                	push   $0x1
 5c5:	57                   	push   %edi
 5c6:	56                   	push   %esi
 5c7:	e8 83 fe ff ff       	call   44f <write>
 5cc:	8b 4d d4             	mov    -0x2c(%ebp),%ecx
      } else {
        putc(fd, c);
 5cf:	83 c4 10             	add    $0x10,%esp
  for(i = 0; fmt[i]; i++){
 5d2:	0f b6 13             	movzbl (%ebx),%edx
 5d5:	83 c3 01             	add    $0x1,%ebx
 5d8:	84 d2                	test   %dl,%dl
 5da:	74 24                	je     600 <printf+0x80>
    c = fmt[i] & 0xff;
 5dc:	0f b6 c2             	movzbl %dl,%eax
    if(state == 0){
 5df:	85 c9                	test   %ecx,%ecx
 5e1:	74 cd                	je     5b0 <printf+0x30>
      }
    } else if(state == '%'){
 5e3:	83 f9 25             	cmp    $0x25,%ecx
 5e6:	75 ea                	jne    5d2 <printf+0x52>
      if(c == 'd'){
 5e8:	83 f8 25             	cmp    $0x25,%eax
 5eb:	0f 84 07 01 00 00    	je     6f8 <printf+0x178>
 5f1:	83 e8 63             	sub    $0x63,%eax
 5f4:	83 f8 15             	cmp    $0x15,%eax
 5f7:	77 17                	ja     610 <printf+0x90>
 5f9:	ff 24 85 00 09 00 00 	jmp    *0x900(,%eax,4)
        putc(fd, c);
      }
      state = 0;
    }
  }
}
 600:	8d 65 f4             	lea    -0xc(%ebp),%esp
 603:	5b                   	pop    %ebx
 604:	5e                   	pop    %esi
 605:	5f                   	pop    %edi
 606:	5d                   	pop    %ebp
 607:	c3                   	ret    
 608:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 60f:	90                   	nop
  write(fd, &c, 1);
 610:	83 ec 04             	sub    $0x4,%esp
 613:	88 55 d4             	mov    %dl,-0x2c(%ebp)
 616:	6a 01                	push   $0x1
 618:	57                   	push   %edi
 619:	56                   	push   %esi
 61a:	c6 45 e7 25          	movb   $0x25,-0x19(%ebp)
 61e:	e8 2c fe ff ff       	call   44f <write>
        putc(fd, c);
 623:	0f b6 55 d4          	movzbl -0x2c(%ebp),%edx
  write(fd, &c, 1);
 627:	83 c4 0c             	add    $0xc,%esp
 62a:	88 55 e7             	mov    %dl,-0x19(%ebp)
 62d:	6a 01                	push   $0x1
 62f:	57                   	push   %edi
 630:	56                   	push   %esi
 631:	e8 19 fe ff ff       	call   44f <write>
        putc(fd, c);
 636:	83 c4 10             	add    $0x10,%esp
      state = 0;
 639:	31 c9                	xor    %ecx,%ecx
 63b:	eb 95                	jmp    5d2 <printf+0x52>
 63d:	8d 76 00             	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
 640:	83 ec 0c             	sub    $0xc,%esp
 643:	b9 10 00 00 00       	mov    $0x10,%ecx
 648:	6a 00                	push   $0x0
 64a:	8b 45 d0             	mov    -0x30(%ebp),%eax
 64d:	8b 10                	mov    (%eax),%edx
 64f:	89 f0                	mov    %esi,%eax
 651:	e8 7a fe ff ff       	call   4d0 <printint>
        ap++;
 656:	83 45 d0 04          	addl   $0x4,-0x30(%ebp)
 65a:	83 c4 10             	add    $0x10,%esp
      state = 0;
 65d:	31 c9                	xor    %ecx,%ecx
 65f:	e9 6e ff ff ff       	jmp    5d2 <printf+0x52>
 664:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
 668:	8b 45 d0             	mov    -0x30(%ebp),%eax
 66b:	8b 10                	mov    (%eax),%edx
        ap++;
 66d:	83 c0 04             	add    $0x4,%eax
 670:	89 45 d0             	mov    %eax,-0x30(%ebp)
        if(s == 0)
 673:	85 d2                	test   %edx,%edx
 675:	0f 84 8d 00 00 00    	je     708 <printf+0x188>
        while(*s != 0){
 67b:	0f b6 02             	movzbl (%edx),%eax
      state = 0;
 67e:	31 c9                	xor    %ecx,%ecx
        while(*s != 0){
 680:	84 c0                	test   %al,%al
 682:	0f 84 4a ff ff ff    	je     5d2 <printf+0x52>
 688:	89 5d d4             	mov    %ebx,-0x2c(%ebp)
 68b:	89 d3                	mov    %edx,%ebx
 68d:	8d 76 00             	lea    0x0(%esi),%esi
  write(fd, &c, 1);
 690:	83 ec 04             	sub    $0x4,%esp
          s++;
 693:	83 c3 01             	add    $0x1,%ebx
 696:	88 45 e7             	mov    %al,-0x19(%ebp)
  write(fd, &c, 1);
 699:	6a 01                	push   $0x1
 69b:	57                   	push   %edi
 69c:	56                   	push   %esi
 69d:	e8 ad fd ff ff       	call   44f <write>
        while(*s != 0){
 6a2:	0f b6 03             	movzbl (%ebx),%eax
 6a5:	83 c4 10             	add    $0x10,%esp
 6a8:	84 c0                	test   %al,%al
 6aa:	75 e4                	jne    690 <printf+0x110>
      state = 0;
 6ac:	8b 5d d4             	mov    -0x2c(%ebp),%ebx
 6af:	31 c9                	xor    %ecx,%ecx
 6b1:	e9 1c ff ff ff       	jmp    5d2 <printf+0x52>
 6b6:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 6bd:	8d 76 00             	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
 6c0:	83 ec 0c             	sub    $0xc,%esp
 6c3:	b9 0a 00 00 00       	mov    $0xa,%ecx
 6c8:	6a 01                	push   $0x1
 6ca:	e9 7b ff ff ff       	jmp    64a <printf+0xca>
 6cf:	90                   	nop
        putc(fd, *ap);
 6d0:	8b 45 d0             	mov    -0x30(%ebp),%eax
  write(fd, &c, 1);
 6d3:	83 ec 04             	sub    $0x4,%esp
        putc(fd, *ap);
 6d6:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
 6d8:	6a 01                	push   $0x1
 6da:	57                   	push   %edi
 6db:	56                   	push   %esi
        putc(fd, *ap);
 6dc:	88 45 e7             	mov    %al,-0x19(%ebp)
  write(fd, &c, 1);
 6df:	e8 6b fd ff ff       	call   44f <write>
        ap++;
 6e4:	83 45 d0 04          	addl   $0x4,-0x30(%ebp)
 6e8:	83 c4 10             	add    $0x10,%esp
      state = 0;
 6eb:	31 c9                	xor    %ecx,%ecx
 6ed:	e9 e0 fe ff ff       	jmp    5d2 <printf+0x52>
 6f2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        putc(fd, c);
 6f8:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
 6fb:	83 ec 04             	sub    $0x4,%esp
 6fe:	e9 2a ff ff ff       	jmp    62d <printf+0xad>
 703:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 707:	90                   	nop
          s = "(null)";
 708:	ba f8 08 00 00       	mov    $0x8f8,%edx
        while(*s != 0){
 70d:	89 5d d4             	mov    %ebx,-0x2c(%ebp)
 710:	b8 28 00 00 00       	mov    $0x28,%eax
 715:	89 d3                	mov    %edx,%ebx
 717:	e9 74 ff ff ff       	jmp    690 <printf+0x110>
 71c:	66 90                	xchg   %ax,%ax
 71e:	66 90                	xchg   %ax,%ax

00000720 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 720:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 721:	a1 78 0c 00 00       	mov    0xc78,%eax
{
 726:	89 e5                	mov    %esp,%ebp
 728:	57                   	push   %edi
 729:	56                   	push   %esi
 72a:	53                   	push   %ebx
 72b:	8b 5d 08             	mov    0x8(%ebp),%ebx
  bp = (Header*)ap - 1;
 72e:	8d 4b f8             	lea    -0x8(%ebx),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 731:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 738:	89 c2                	mov    %eax,%edx
 73a:	8b 00                	mov    (%eax),%eax
 73c:	39 ca                	cmp    %ecx,%edx
 73e:	73 30                	jae    770 <free+0x50>
 740:	39 c1                	cmp    %eax,%ecx
 742:	72 04                	jb     748 <free+0x28>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 744:	39 c2                	cmp    %eax,%edx
 746:	72 f0                	jb     738 <free+0x18>
      break;
  if(bp + bp->s.size == p->s.ptr){
 748:	8b 73 fc             	mov    -0x4(%ebx),%esi
 74b:	8d 3c f1             	lea    (%ecx,%esi,8),%edi
 74e:	39 f8                	cmp    %edi,%eax
 750:	74 30                	je     782 <free+0x62>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
 752:	89 43 f8             	mov    %eax,-0x8(%ebx)
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
 755:	8b 42 04             	mov    0x4(%edx),%eax
 758:	8d 34 c2             	lea    (%edx,%eax,8),%esi
 75b:	39 f1                	cmp    %esi,%ecx
 75d:	74 3a                	je     799 <free+0x79>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
 75f:	89 0a                	mov    %ecx,(%edx)
  } else
    p->s.ptr = bp;
  freep = p;
}
 761:	5b                   	pop    %ebx
  freep = p;
 762:	89 15 78 0c 00 00    	mov    %edx,0xc78
}
 768:	5e                   	pop    %esi
 769:	5f                   	pop    %edi
 76a:	5d                   	pop    %ebp
 76b:	c3                   	ret    
 76c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 770:	39 c2                	cmp    %eax,%edx
 772:	72 c4                	jb     738 <free+0x18>
 774:	39 c1                	cmp    %eax,%ecx
 776:	73 c0                	jae    738 <free+0x18>
  if(bp + bp->s.size == p->s.ptr){
 778:	8b 73 fc             	mov    -0x4(%ebx),%esi
 77b:	8d 3c f1             	lea    (%ecx,%esi,8),%edi
 77e:	39 f8                	cmp    %edi,%eax
 780:	75 d0                	jne    752 <free+0x32>
    bp->s.size += p->s.ptr->s.size;
 782:	03 70 04             	add    0x4(%eax),%esi
 785:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 788:	8b 02                	mov    (%edx),%eax
 78a:	8b 00                	mov    (%eax),%eax
 78c:	89 43 f8             	mov    %eax,-0x8(%ebx)
  if(p + p->s.size == bp){
 78f:	8b 42 04             	mov    0x4(%edx),%eax
 792:	8d 34 c2             	lea    (%edx,%eax,8),%esi
 795:	39 f1                	cmp    %esi,%ecx
 797:	75 c6                	jne    75f <free+0x3f>
    p->s.size += bp->s.size;
 799:	03 43 fc             	add    -0x4(%ebx),%eax
  freep = p;
 79c:	89 15 78 0c 00 00    	mov    %edx,0xc78
    p->s.size += bp->s.size;
 7a2:	89 42 04             	mov    %eax,0x4(%edx)
    p->s.ptr = bp->s.ptr;
 7a5:	8b 4b f8             	mov    -0x8(%ebx),%ecx
 7a8:	89 0a                	mov    %ecx,(%edx)
}
 7aa:	5b                   	pop    %ebx
 7ab:	5e                   	pop    %esi
 7ac:	5f                   	pop    %edi
 7ad:	5d                   	pop    %ebp
 7ae:	c3                   	ret    
 7af:	90                   	nop

000007b0 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 7b0:	55                   	push   %ebp
 7b1:	89 e5                	mov    %esp,%ebp
 7b3:	57                   	push   %edi
 7b4:	56                   	push   %esi
 7b5:	53                   	push   %ebx
 7b6:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 7b9:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 7bc:	8b 3d 78 0c 00 00    	mov    0xc78,%edi
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 7c2:	8d 70 07             	lea    0x7(%eax),%esi
 7c5:	c1 ee 03             	shr    $0x3,%esi
 7c8:	83 c6 01             	add    $0x1,%esi
  if((prevp = freep) == 0){
 7cb:	85 ff                	test   %edi,%edi
 7cd:	0f 84 9d 00 00 00    	je     870 <malloc+0xc0>
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 7d3:	8b 17                	mov    (%edi),%edx
    if(p->s.size >= nunits){
 7d5:	8b 4a 04             	mov    0x4(%edx),%ecx
 7d8:	39 f1                	cmp    %esi,%ecx
 7da:	73 6a                	jae    846 <malloc+0x96>
 7dc:	bb 00 10 00 00       	mov    $0x1000,%ebx
 7e1:	39 de                	cmp    %ebx,%esi
 7e3:	0f 43 de             	cmovae %esi,%ebx
  p = sbrk(nu * sizeof(Header));
 7e6:	8d 04 dd 00 00 00 00 	lea    0x0(,%ebx,8),%eax
 7ed:	89 45 e4             	mov    %eax,-0x1c(%ebp)
 7f0:	eb 17                	jmp    809 <malloc+0x59>
 7f2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 7f8:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 7fa:	8b 48 04             	mov    0x4(%eax),%ecx
 7fd:	39 f1                	cmp    %esi,%ecx
 7ff:	73 4f                	jae    850 <malloc+0xa0>
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 801:	8b 3d 78 0c 00 00    	mov    0xc78,%edi
 807:	89 c2                	mov    %eax,%edx
 809:	39 d7                	cmp    %edx,%edi
 80b:	75 eb                	jne    7f8 <malloc+0x48>
  p = sbrk(nu * sizeof(Header));
 80d:	83 ec 0c             	sub    $0xc,%esp
 810:	ff 75 e4             	push   -0x1c(%ebp)
 813:	e8 9f fc ff ff       	call   4b7 <sbrk>
  if(p == (char*)-1)
 818:	83 c4 10             	add    $0x10,%esp
 81b:	83 f8 ff             	cmp    $0xffffffff,%eax
 81e:	74 1c                	je     83c <malloc+0x8c>
  hp->s.size = nu;
 820:	89 58 04             	mov    %ebx,0x4(%eax)
  free((void*)(hp + 1));
 823:	83 ec 0c             	sub    $0xc,%esp
 826:	83 c0 08             	add    $0x8,%eax
 829:	50                   	push   %eax
 82a:	e8 f1 fe ff ff       	call   720 <free>
  return freep;
 82f:	8b 15 78 0c 00 00    	mov    0xc78,%edx
      if((p = morecore(nunits)) == 0)
 835:	83 c4 10             	add    $0x10,%esp
 838:	85 d2                	test   %edx,%edx
 83a:	75 bc                	jne    7f8 <malloc+0x48>
        return 0;
  }
}
 83c:	8d 65 f4             	lea    -0xc(%ebp),%esp
        return 0;
 83f:	31 c0                	xor    %eax,%eax
}
 841:	5b                   	pop    %ebx
 842:	5e                   	pop    %esi
 843:	5f                   	pop    %edi
 844:	5d                   	pop    %ebp
 845:	c3                   	ret    
    if(p->s.size >= nunits){
 846:	89 d0                	mov    %edx,%eax
 848:	89 fa                	mov    %edi,%edx
 84a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      if(p->s.size == nunits)
 850:	39 ce                	cmp    %ecx,%esi
 852:	74 4c                	je     8a0 <malloc+0xf0>
        p->s.size -= nunits;
 854:	29 f1                	sub    %esi,%ecx
 856:	89 48 04             	mov    %ecx,0x4(%eax)
        p += p->s.size;
 859:	8d 04 c8             	lea    (%eax,%ecx,8),%eax
        p->s.size = nunits;
 85c:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
 85f:	89 15 78 0c 00 00    	mov    %edx,0xc78
}
 865:	8d 65 f4             	lea    -0xc(%ebp),%esp
      return (void*)(p + 1);
 868:	83 c0 08             	add    $0x8,%eax
}
 86b:	5b                   	pop    %ebx
 86c:	5e                   	pop    %esi
 86d:	5f                   	pop    %edi
 86e:	5d                   	pop    %ebp
 86f:	c3                   	ret    
    base.s.ptr = freep = prevp = &base;
 870:	c7 05 78 0c 00 00 7c 	movl   $0xc7c,0xc78
 877:	0c 00 00 
    base.s.size = 0;
 87a:	bf 7c 0c 00 00       	mov    $0xc7c,%edi
    base.s.ptr = freep = prevp = &base;
 87f:	c7 05 7c 0c 00 00 7c 	movl   $0xc7c,0xc7c
 886:	0c 00 00 
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 889:	89 fa                	mov    %edi,%edx
    base.s.size = 0;
 88b:	c7 05 80 0c 00 00 00 	movl   $0x0,0xc80
 892:	00 00 00 
    if(p->s.size >= nunits){
 895:	e9 42 ff ff ff       	jmp    7dc <malloc+0x2c>
 89a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        prevp->s.ptr = p->s.ptr;
 8a0:	8b 08                	mov    (%eax),%ecx
 8a2:	89 0a                	mov    %ecx,(%edx)
 8a4:	eb b9                	jmp    85f <malloc+0xaf>
