#!/bin/sh

CC=gcc
AS=as
LD=ld
OBJCOPY=objcopy
STRIP=strip

# build info

WHO=$(whoami)
WHERE=$(hostname)
ARCH=$(uname -m)
OS=$(uname -o)

cat <<!EOF
# file generated by $0

WHEN			= \$(shell LANG=C date)
WHO			= ${WHO}
WHERE			= ${WHERE}
ARCH			= ${ARCH}
OS			= ${OS}

SIGNATURE = \$(PACKAGE)-\$(VERSION) \$(WHO)@\$(WHERE)(\$(ARCH) \$(OS)) \$(WHEN)
!EOF

# m68k cross-compiler

if test "${ARCH}" != "m68k"
then
	M68K_CROSS_COMPILE=m68k-linux-
	if ! type "${M68K_CROSS_COMPILE}${CC}" > /dev/null 2>&1
	then
		M68K_CROSS_COMPILE=m68k-linux-gnu-
		if ! type "${M68K_CROSS_COMPILE}${CC}" > /dev/null 2>&1
		then
			echo "Cannot find m68k cross-compiler" 1>&2
			exit 1
		fi
	fi
	M68K_GCC_VERSION=$(${M68K_CROSS_COMPILE}${CC} -dumpversion 2> /dev/null)
	echo "cross-compiler is ${M68K_CROSS_COMPILE}${CC} ${M68K_GCC_VERSION}" 1>&2
fi

cat <<!EOF

M68K_CROSS_COMPILE = ${M68K_CROSS_COMPILE}

M68K_AS			= \$(M68K_CROSS_COMPILE)${AS}
M68K_CC			= \$(M68K_CROSS_COMPILE)${CC}
M68K_LD			= \$(M68K_CROSS_COMPILE)${LD}
M68K_OBJCOPY		= \$(M68K_CROSS_COMPILE)${OBJCOPY}
M68K_STRIP		= \$(M68K_CROSS_COMPILE)${STRIP}
!EOF

if test "${ARCH}" != "ppc"
then
	PPC_CROSS_COMPILE=powerpc-linux-
	if ! type "${PPC_CROSS_COMPILE}${CC}" > /dev/null 2>&1
	then
		PPC_CROSS_COMPILE=powerpc-linux-gnu-
		if ! type "${PPC_CROSS_COMPILE}${CC}" > /dev/null 2>&1
		then
			echo "Cannot find powerpc cross-compiler" 1>&2
		else
			PPC_GCC_VERSION=$(${PPC_CROSS_COMPILE}${CC} -dumpversion 2> /dev/null)
			echo "cross-compiler is ${PPC_CROSS_COMPILE}${CC} ${PPC_GCC_VERSION}" 1>&2
cat <<!EOF

PPC_CROSS_COMPILE = ${PPC_CROSS_COMPILE}

PPC_AS		= \$(PPC_CROSS_COMPILE)${AS}
PPC_CC		= \$(PPC_CROSS_COMPILE)${CC}
PPC_LD		= \$(PPC_CROSS_COMPILE)${LD}
PPC_OBJCOPY	= \$(PPC_CROSS_COMPILE)${OBJCOPY}
PPC_STRIP	= \$(PPC_CROSS_COMPILE)${STRIP}
!EOF
		fi
	fi
fi

# target compiler

cat <<!EOF

ifeq (\$(TARGET),m68k-linux)

override AS		= \$(M68K_AS)
override CC		= \$(M68K_CC)
override LD		= \$(M68K_LD)
override OBJCOPY	= \$(M68K_OBJCOPY)
override STRIP		= \$(M68K_STRIP)

else ifeq (\$(TARGET),m68k-netbsd)

override AS		= \$(M68K_AS)
override CC		= \$(M68K_CC)
override LD		= \$(M68K_LD)
override OBJCOPY	= \$(M68K_OBJCOPY)
override STRIP		= \$(M68K_STRIP)

!EOF

if type "${PPC_CROSS_COMPILE}${CC}" > /dev/null 2>&1
then
cat <<!EOF
else ifeq (\$(TARGET),ppc-linux)

override AS		= \$(PPC_AS)
override CC		= \$(PPC_CC)
override LD		= \$(PPC_LD)
override OBJCOPY	= \$(PPC_OBJCOPY)
override STRIP		= \$(PPC_STRIP)

!EOF
fi

cat <<!EOF
else

AS		= \$(CROSS_COMPILE)${AS}
CC		= \$(CROSS_COMPILE)${CC}
LD		= \$(CROSS_COMPILE)${LD}
OBJCOPY		= \$(CROSS_COMPILE)${OBJCOPY}
STRIP		= \$(CROSS_COMPILE)${STRIP}

endif
!EOF

# docbook to man

if type docbook-to-man > /dev/null 2>&1
then
cat <<!EOF
 
%.8: %.sgml
	docbook-to-man \$< > \$@
!EOF
else
if type ${docbook-to-man} > /dev/null 2>&1
then
cat <<!EOF
 
%.8: %.sgml
	docbook2man \$<
!EOF
else
 
%.8: %.sgml
	echo "Missing tools to generate \$@ from \$<"
fi
fi