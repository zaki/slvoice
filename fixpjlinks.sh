#!/bin/bash
# makes platform independent archive links
# for unix systems

LIBDIR=${1:-usr/local/lib}
pushd ${LIBDIR}

LIBS=$(find . -name 'lib*.a')
for lib in ${LIBS}
do
    name=$(echo ${lib} | sed -e "s/-.*\.a$/.a/")
    if [ ! -e ${name} ]
    then
        ln -s ${lib} ${name}
    fi
done
popd
