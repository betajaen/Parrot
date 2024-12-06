/* build.rexx */
/*
	Build tool for Parrot
	https://github.com/betajaen/parrot
*/

OPTIONS FAILAT 5
ARG project_file force

IF LENGTH(project_file) = 0 THEN DO
    SAY "Project file not specified!"
    EXIT 20
END

IF EXISTS(project_file) = 0 THEN DO
    SAY "Project file not found!"
    EXIT 20
END

force_build = 0
IF LENGTH(force) ~=0 THEN DO
    force = STRIP(force)
    IF force = 'FORCE' THEN DO
        force_build = 1
        SAY 'Forcing build ...'
    END
END

delim = LASTPOS('/', project_file)
IF delim = 0 THEN DO
    delim = LASTPOS(':', project_file)
    IF delim = 0 THEN DO
        SAY 'Project file should be a absolute path.'
        EXIT 20
    END
END

project_dir = SUBSTR(project_file, 1,  delim)

rv = ''
temp_base = 'T:'
project.name = 'parrot'
project.filename = 'T:parrot'
project.base = project_dir || 'Code'
project.c_file.0 = 0
project.o_file.0 = 0
project.libs.0 = 0
project.defines.0 = 0
project.options.0 = 0

IF ~OPEN('PF', project_file, 'R') THEN DO
    SAY 'Could not open project file: ' || project_file
    EXIT 10
END

DO WHILE ~EOF('PF')
    ll = READLN('PF')

    IF ll ~= '' THEN DO
        lt = SUBSTR(ll, 1, 1)
        IF lt = 'N' THEN DO
        	project.name = STRIP(SUBSTR(ll, 3))
            project.filename = temp_base || project.name
        END
        IF lt = 'B' THEN DO
            project.base = project_dir || STRIP(SUBSTR(ll, 3))
        END
        IF lt = 'F' THEN DO
            idx = 1 + project.c_file.0
            f_name = STRIP(SUBSTR(ll, 3))
            o_name = LEFT(f_name, LENGTH(f_name) - 1) || 'o'
            project.c_file.idx = f_name
            project.o_file.idx = o_name
            project.c_file.0 = idx
            project.o_file.0 = idx
        END
        IF lt = 'D' THEN DO
            idx = 1 + project.defines.0
            project.defines.idx = STRIP(SUBSTR(ll, 3))
            project.defines.0 = idx
        END
        IF lt = 'L' THEN DO
            idx = 1 + project.libs.0
            project.libs.idx = STRIP(SUBSTR(ll, 3))
            project.libs.0 = idx
        END
        IF lt = 'O' THEN DO
            idx = 1 + project.options.0
            project.options.idx = SUBSTR(ll, 3)
            project.options.0 = idx
        END
    END
END

CALL CLOSE('PF')

cc = 'vc +aos68k '
cflags = ''
        
DO i = 1 TO project.libs.0
    cflags = cflags || ' -l' || project.libs.i
END

DO i = 1 TO project.defines.0
    cflags = cflags || ' -D' || project.defines.i
END

DO i = 1 TO project.options.0
    cflags = cflags || ' -' || project.options.i
END

any = 0

OPTIONS FAILAT 50

DO i = 1 TO project.c_file.0
    cf = project.c_file.i
    of = project.o_file.i
    df = of || '.date'
   
    fd = STRIP(UPPER(DateFile(project.base, cf)))
    od = STRIP(UPPER(ReadFile(temp_base || df, '01-Jan-01 01:01:40')))
    IF fd = od THEN DO
        IF force_build = 0 THEN DO
        	SAY cf || ' has not changed.'
        	ITERATE i
        END
    END
                                                  

    SAY 'Compiling ' || cf || ' ...' 

    /* Remove old object file. Incase it 'passes' and attempts to link to an old version. */
    ADDRESS command 'delete ' || temp_base || of || ' >NIL:'

    /* Compile source as object file */
    ocmd = cc ||  ' -c ' || project.base || cf || cflags || ' -o ' || temp_base || of
    SAY '> ' || ocmd
	ADDRESS command ocmd
    IF RC ~= 0 THEN DO
    	SAY '> ' || RC || '.'
        EXIT 0
    END

    any = 1
    CALL WriteFile(temp_base || df, fd)
END

OPTIONS FAILAT 5

IF any == 0 THEN DO
    SAY "Project upto date."
	EXIT 0
END

lcmd = cc

DO i = 1 TO project.o_file.0
    lcmd = lcmd || ' ' || temp_base || project.o_file.i
END

lcmd = lcmd || cflags
lcmd = lcmd || ' -o ' || project.filename

SAY 'Linking ' || project.name || ' ...'

ADDRESS command lcmd

IF RC ~= 0 THEN DO
	SAY '> ' || RC || '.'
    EXIT 0
END
ELSE DO
    SAY 'Linked.'
END

EXIT


/* Date File */                                             
DateFile: PROCEDURE EXPOSE rv
	ARG dirName, fileName
	cmd_str = 'C:List NOHEAD DATES LFORMAT="%D %T" DIR=' || dirName || ' P=' || fileName || " > PIPE:xxb"
    ADDRESS command cmd_str
    OPEN('ppxp', 'PIPE:xxb', 'r')
    p0 = Readln('ppxp')
    CLOSE('ppxp')
    rv = STRIP(p0)
    IF LENGTH(rv) = 0 THEN DO; rv = "01-Feb-03 04:05:06"; END
    RETURN rv

/* Write String to file path */
WriteFile: PROCEDURE
    ARG path, text
    IF OPEN('wf', path, 'w') THEN
        DO
        	WRITELN('wf', text)
        	CLOSE('wf')
        END
    ELSE
    	DO
            SAY 'Couldnt open file ' || path || ' for writing!'
            EXIT 10
        END
    RETURN 0

/* Read file to string */
ReadFile: PROCEDURE EXPOSE rv
	ARG path, defaultValue
    p0 = defaultValue
    IF OPEN('rf', path, 'r') THEN DO
    	p0 = ReadCh('rf', 65535)
    	CLOSE('rf')
    END
    rv = STRIP(p0, 'b', ' ' || '09'x || '0A'x || '0D'x)
    RETURN rv

