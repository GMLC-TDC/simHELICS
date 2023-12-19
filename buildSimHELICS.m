function buildSimHELICS(targetPath,makePackage)
  % buildSimHELICS(targetPath,makePackage) will generate the files
  % necessary for the Simulink HELICS interface.  It will download additional
  % files from github if needed.
  % buildSimHELICS() will generate the package files in the current
  % directory
  % buildSimHELICS(targetPath) will create the package files in the
  % specified targetPath directory
  % buildSimHELICS('') is equivalent to buildHelicsInterface()
  % buildSimHELICS(targetPath,makePackage) will if makePackage is set
  % to true generate a zip/tar.gz file with all the files that can be copied
  % and extracted on a similar system.
  %
  % To make the helics library accessible anywhere on the matlab path the
  % targetPath folder should be added to the matlab path.  (NOTE: it should
  % not be added with subfolders as all required matlab code is in the main
  % folder or in the +helics folder which matlab will recognize as a
  % package).
  %
  % this file requires matlab 2018a or higher.
  if (nargin==0)
    targetPath=fileparts(mfilename('fullpath'));
  end
  if (nargin<2)
    makePackage=false;
  end
  if (isempty(targetPath))
    targetPath=fileparts(mfilename('fullpath'));
  end
  if (~exist(targetPath,'dir'))
    mkdir(targetPath);
  end
  if (makePackage)
    if (isequal(targetPath,fileparts(mfilename('fullpath'))))
      warning('cannot makePackage if target location is the same as the file origin');
      makePackage=false;
    end
  end
  inputPath=fileparts(mfilename('fullpath'));
  HelicsVersion='3.5.2';
  % set up platform specific names and locations
  targetTarFile=fullfile(targetPath,['helicsTar',HelicsVersion,'.tar.gz']);
  if ismac
    basePath=fullfile(targetPath,['Helics-',HelicsVersion,'-macOS-universal2']);
    baseFile=['Helics-shared-',HelicsVersion,'-macOS-universal2.tar.gz'];
    targetFile=fullfile(basePath,'lib','libhelics.dylib');
    % download the helics library if needed
    if (~exist(fullfile(basePath,'include/helics/helics.h'),'file'))
      if (~exist(targetTarFile,'file'))
        fprintf('downloading helics binary package\n');
        if isOctave
          urlwrite(["https://github.com/GMLC-TDC/HELICS/releases/download/v",HelicsVersion,"/",baseFile],targetTarFile);
        else
          websave(targetTarFile,['https://github.com/GMLC-TDC/HELICS/releases/download/v',HelicsVersion,'/',baseFile]);
        end
      end
      fprintf('extracting helics binary package\n');
      system(['tar xf ',targetTarFile,' -C ',targetPath]);
    end
    %actually build the mex file
    fprintf('building helics mex target\n');
   
   mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['LDFLAGS=$LDFLAGS -Wl,-rpath,$ORIGIN/lib,-rpath,',basePath,'/lib',',-rpath,',basePath,'/lib64'],fullfile(inputPath,'helics_federate.c'),'-outdir',targetPath);
   mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['LDFLAGS=$LDFLAGS -Wl,-rpath,$ORIGIN/lib,-rpath,',basePath,'/lib',',-rpath,',basePath,'/lib64'],fullfile(inputPath,'helics_input.c'),'-outdir',targetPath);
      mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['LDFLAGS=$LDFLAGS -Wl,-rpath,$ORIGIN/lib,-rpath,',basePath,'/lib',',-rpath,',basePath,'/lib64'],fullfile(inputPath,'helics_publication.c'),'-outdir',targetPath);
  elseif isunix
    basePath=fullfile(targetPath,['Helics-',HelicsVersion,'-Linux-x86_64']);
    baseFile=['Helics-shared-',HelicsVersion,'-Linux-x86_64.tar.gz'];
    targetFile=fullfile(basePath,'lib64','libhelics.so');
    % download the helics library if needed
    if (~exist(fullfile(basePath,'include/helics/helics.h'),'file'))
      if (~exist(targetTarFile,'file'))
        fprintf('downloading helics binary package\n');
        websave(targetTarFile,['https://github.com/GMLC-TDC/HELICS/releases/download/v',HelicsVersion,'/',baseFile]);
      end
      fprintf('extracting helics binary package\n');
      system(['tar xf ',targetTarFile,' -C ',targetPath]);
    end
    %actually build the mex file
    fprintf('building helics mex target\n');
    mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['-L',basePath,'/lib64'],['LDFLAGS=$LDFLAGS -Wl,-rpath,$ORIGIN/lib,-rpath,',basePath,'/lib',',-rpath,',basePath,'/lib64'],fullfile(inputPath,'helics_publication.c'),'-outdir',targetPath);
      mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['-L',basePath,'/lib64'],['LDFLAGS=$LDFLAGS -Wl,-rpath,$ORIGIN/lib,-rpath,',basePath,'/lib',',-rpath,',basePath,'/lib64'],fullfile(inputPath,'helics_input.c'),'-outdir',targetPath);
      mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['-L',basePath,'/lib64'],['LDFLAGS=$LDFLAGS -Wl,-rpath,$ORIGIN/lib,-rpath,',basePath,'/lib',',-rpath,',basePath,'/lib64'],fullfile(inputPath,'helics_federate.c'),'-outdir',targetPath);
  elseif ispc
    if isequal(computer,'PCWIN64')
      basePath=fullfile(targetPath,['Helics-',HelicsVersion,'-win64']);
      baseFile=['Helics-shared-',HelicsVersion,'-win64.tar.gz'];
      targetFile='helics.dll';
    else
      basePath=fullfile(targetPath,['Helics-',HelicsVersion,'-win32']);
      baseFile=['Helics-shared-',HelicsVersion,'-win32.tar.gz'];
      targetFile='helics.dll';
    end
    % download the helics library if needed
    if (~exist(fullfile(basePath,'include/helics/helics.h'),'file'))
      if (~exist(targetTarFile,'file'))
        fprintf('downloading helics binary package\n');
        websave(targetTarFile,['https://github.com/GMLC-TDC/HELICS/releases/download/v',HelicsVersion,'/',baseFile]);
      end
      fprintf('extracting helics binary package\n');
      untar(targetTarFile,targetPath);
    end
    %actually build the mex file
    fprintf('building helics simulink targets\n');
      mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['-L',basePath,'/bin'],fullfile(inputPath,'helics_federate.c'),'-outdir',targetPath);
      mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['-L',basePath,'/bin'],fullfile(inputPath,'helics_input.c'),'-outdir',targetPath);
      mex('-lhelics','-R2018a',['-I',basePath,'/include/'],['-L',basePath,'/lib'],['-L',basePath,'/bin'],fullfile(inputPath,'helics_publication.c'),'-outdir',targetPath);
    %copy the needed dll file if on windows
    if ispc
      if (~exist(fullfile(targetPath,targetFile),'file'))
        
          copyfile(fullfile(basePath,'bin',targetFile),fullfile(targetPath,targetFile));
          copyfile(fullfile(basePath,'bin','*.dll'),targetPath);
      end
    end
  else
    error('Platform not supported');
  end
  %% now build the interface directory and copy files
  fprintf('copying required files\n');
  
  % copy the include directory with the C headers
  if (~exist(fullfile(targetPath,'include'),'dir'))
    mkdir(fullfile(targetPath,'include'));
  end
  copyfile(fullfile(inputPath,'helics_minimal.h'),fullfile(targetPath,'include','helics_minimal.h'));
  if (ismac)
    [status, result]=system(['cp -R ',fullfile(basePath,'lib'),' ',fullfile(targetPath,'lib')]);
    if (status~=0)
      disp(result);
    end
  elseif (isunix)
    [status, result]=system(['cp -R ',fullfile(basePath,'lib64'),' ',fullfile(targetPath,'lib')]);
    if (status~=0)
      disp(result);
    end
  else
    copyfile(fullfile(basePath,'bin'),fullfile(targetPath,'bin'));
  end
  
  if (makePackage)
    fprintf('generating helics simulink binary package file\n');
    rmdir(basePath,'s');
    delete(targetTarFile);
    if ismac || isunix
      system(['tar czf simHELICS.tar.gz ',fullfile(targetPath,'*')]);
    elseif ispc
      zip('simHELICS','*',targetPath);
    else
      warning('unrecognized platform for making package');
    end
  end



