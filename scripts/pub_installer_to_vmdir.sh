# 打包生成安装包并复制到虚拟机共享目录

# 获取父目录地址
SOURCEDIR="$(
  cd "$(dirname "$0")/../"
  pwd -P
)"
BUILDDIR="${SOURCEDIR}/build"
SCRIPTDIR="${SOURCEDIR}/scripts"
# 调用copy脚本复制依赖库
echo "调用copy脚本复制依赖库......"
cd ${SCRIPTDIR}
/c/msys64/msys2_shell.cmd -ucrt64 ./copy_dlls_to_install.sh
echo "[完成]"
read -p '按回车继续...' var
# cpack打包生成
echo "cpack打包生成......"
cd ${BUILDDIR}
cpack
echo "[完成]"
# 复制安装包到共享目录
echo "复制安装包到共享目录......"
cp ${BUILDDIR}/AndroidSystemSimplify*zip ${VM_SHARE_DIR}
echo "[完成]"
read -p '按回车继续...' var