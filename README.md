# OpenMPI MIET 
Данный репозиторий представляет из себя лабораторные работы
по дисциплине "Технологии параллельного программирования"
в ВУЗ "МИЭТ"    
- [Установка OpenMPI Linux в CLion](#установка-openmpi-в-clion-на-linux-)

## Установка OpenMPI в CLion на linux:
___
### Установите OpenMPI в систему:
```bash
sudo apt-get install libopenmpi-dev
```
### Добавьте конфигурацию в `CMakeLists.txt`
Перед `add_executable` вставьте
```Cmake 
find_package(MPI REQUIRED)
```
После
```Cmake 
target_link_libraries(Name_Project PRIVATE MPI::MPI_CXX)
```


