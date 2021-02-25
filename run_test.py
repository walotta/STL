import os

DataName = ['one', 'two', 'three']
for item in DataName:
    check = input('this is ' + item + ' test, enter y/n to run : ')
    if check != 'y':
        continue
    os.system('cp ../storage/' + item + '/code.cpp run_cpp.cpp')
    os.system('cp ../storage/' + item + '/answer.txt answer.txt')
    print('======begin buildingProgram======')
    os.system('cmake CMakeLists.txt > log.txt')
    os.system('make > log.txt')
    print('======begin running======\n')
    os.system('./test > run_ans.txt')
    print('======begin diff======')
    os.system('diff run_ans.txt answer.txt')
    print('======diff end======\n')

memcheckFlag = input('Do you want to do memcheck [y/n/q] : ')
if memcheckFlag == 'y':
    for item in DataName:
        check = input('this is ' + item + ' memcheck test, enter y/n to run : ')
        if check != 'y':
            continue
        os.system('cp ../storage/' + item + '/code.cpp run_cpp.cpp')
        os.system('cp ../storage/' + item + '/answer.txt answer.txt')
        print('======begin buildingProgram======')
        os.system('cmake CMakeLists.txt > log.txt')
        os.system('make > log.txt')
        print('======begin running valgrind======')
        os.system('valgrind --tool=memcheck --leak-check=full ./test')
        print('======valgrind end======\n')
if memcheckFlag == 'q':
    print('run memcheck quiet now')
    for item in DataName:
        check = input('this is ' + item + ' memcheck test, enter y/n to run : ')
        if check != 'y':
            continue
        os.system('cp ../storage/' + item + '/code.cpp run_cpp.cpp')
        os.system('cp ../storage/' + item + '/answer.txt answer.txt')
        print('======begin buildingProgram======')
        os.system('cmake CMakeLists.txt > log.txt')
        os.system('make > log.txt')
        print('======begin running valgrind======')
        os.system('valgrind --tool=memcheck --leak-check=full -q ./test ')
        print('======valgrind end======\n')
