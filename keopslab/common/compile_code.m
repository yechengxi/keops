function testbuild = compile_code(cmd_cmake, cmd_make, filename, msg)

    [src_dir, build_dir, precision, verbosity, use_cuda_if_possible] = default_options();
    
    fprintf(['Compiling ', filename, ' in ', build_dir, msg, '\n        dtype  : ', precision, '\n ... '])

    % it seems to be a workaround to flush Matlab's default LD_LIBRARY_PATH
    setenv('LD_LIBRARY_PATH','') 
    % I do not have a better option to set working dir...
    cur_dir= pwd; cd(build_dir) ;
    % find cmake :
    cmake = getcmake();
    % cmake command:
    cmdline = [cmake, ' ', src_dir , ' -DUSE_CUDA=', num2str(use_cuda_if_possible), ...
               ' -DCMAKE_BUILD_TYPE=Release', ' -D__TYPE__=', precision, ...
               ' -DMatlab_ROOT_DIR="', matlabroot, '" ', cmd_cmake];
    
    try
        
        [prebuild_status,prebuild_output] = system(cmdline);
        if (verbosity ==1) || (prebuild_status ~=0)
            fprintf('\n\n-------------------------------------  DEBUG  ------------------------------------------\n\n');
            disp(prebuild_output)
            fprintf('\n\n-----------------------------------  END DEBUG  ----------------------------------------\n\n');
        end
            
        [build_status,build_output] = system(['make ', cmd_make]);
        if (verbosity ==1) || (build_status ~= 0)
            fprintf('\n\n-------------------------------------  DEBUG  ------------------------------------------\n\n');
            disp(build_output)
            fprintf('\n\n-----------------------------------  END DEBUG  ----------------------------------------\n\n');
        end
        
    catch
        
        cd(cur_dir)
        error('Compilation  Failed')
    
    end
    
    % ...coming back to current directory
    cd(cur_dir)

    testbuild = (exist([filename,'.',mexext],'file')==3);
    if  testbuild
        fprintf('Done.\n')
    else
        error(['File "',filename,'.',mexext, '" not found!'])
    end
end


