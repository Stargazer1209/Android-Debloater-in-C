#!/usr/bin/env sh
#shell script using msys2 ucrt64 env to extract dependencies

#将工作目录切换到父目录
SOURCEDIR="$(
  cd "$(dirname "$0")/../"
  pwd -P
)"
TARGETDIR="${SOURCEDIR}/install"
PROJECTNAME="AndroidSystemSimplify"
ANDROID="/d/0PROGRAM/CMD/android-sdk"

echo -n "清空先前遗留的文件......"
rm -rf ${TARGETDIR}/
mkdir ${TARGETDIR}
mkdir ${TARGETDIR}/bin
mkdir ${TARGETDIR}/lib
mkdir ${TARGETDIR}/share
mkdir ${TARGETDIR}/etc
echo "[完成]"

#
echo -n "复制ADB及其依赖库......"
ldd "${ANDROID}/platform-tools/adb.exe" | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
cp -a "${ANDROID}/platform-tools/." "${TARGETDIR}/bin/"
echo "[完成]"

# 主程序
echo -n "复制主程序第三方依赖库......"
ldd "${SOURCEDIR}/build/${PROJECTNAME}.exe" | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
echo "[完成]"

# GTK套件依赖库
echo -n "复制libegl......"
cp -f $MSYSTEM_PREFIX/bin/libgegl-0.4-0.dll "${TARGETDIR}/bin/"
ldd $MSYSTEM_PREFIX/bin/libgegl-0.4-0.dll | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
echo "[完成]"
#
echo -n "复制 json-glib......"
ldd $MSYSTEM_PREFIX/bin/libjson-glib-1.0-0.dll | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
echo "[完成]"
#
echo -n "复制 gdbus/glib 及其依赖库......"
cp -f $MSYSTEM_PREFIX/bin/gdbus.exe "${TARGETDIR}/bin/"
ldd $MSYSTEM_PREFIX/bin/gdbus.exe | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
cp -f $MSYSTEM_PREFIX/bin/gspawn-win64-helper.exe "${TARGETDIR}/bin/"
ldd $MSYSTEM_PREFIX/bin/gspawn-win64-helper.exe | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
echo "[完成]"
#
echo -n "复制 gdk-pixbuf......"
cp -rf $MSYSTEM_PREFIX/lib/gdk-pixbuf-2.0 "${TARGETDIR}/lib/"
ldd $MSYSTEM_PREFIX/lib/gdk-pixbuf-2.0/2.10.0/loaders/libpixbufloader-svg.dll | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
echo "[完成]"
#
echo -n "复制 gio......"
cp -rf $MSYSTEM_PREFIX/lib/gio "${TARGETDIR}/lib/"
echo "[完成]"
#
echo -n "删除多余静态库......"
find "${TARGETDIR}/lib" -type f -path '*.dll.a' -exec rm '{}' \;
echo "[完成]"

# 复制 gtk 主题资源
echo -n "复制 gtk 主题资源......."
# 图标
mkdir -p "${TARGETDIR}/share/icons"
cp -rf $MSYSTEM_PREFIX/share/icons/Adwaita "${TARGETDIR}/share/icons/"
cp -rf $MSYSTEM_PREFIX/share/icons/hicolor "${TARGETDIR}/share/icons/"
# 设置
cp -rf $MSYSTEM_PREFIX/etc/gtk-4.0 "${TARGETDIR}/etc/"
cp "${SOURCEDIR}/etc/bloatware_data.txt" "${TARGETDIR}/etc/"
# 编译 gsettings xml 文件
glib-compile-schemas $MSYSTEM_PREFIX/share/glib-2.0/schemas
mkdir -p "${TARGETDIR}/share/glib-2.0/schemas"
cp -f $MSYSTEM_PREFIX/share/glib-2.0/schemas/gschema* "${TARGETDIR}/share/glib-2.0/schemas/"
# 主题
mkdir -p "${TARGETDIR}/share/themes"
cp -rLf $MSYSTEM_PREFIX/share/themes/Fluent1 "${TARGETDIR}/share/themes/"
cp -rLf $MSYSTEM_PREFIX/share/themes/Fluent2 "${TARGETDIR}/share/themes/"
cp -rLf $MSYSTEM_PREFIX/share/themes/Fluent2-Dark "${TARGETDIR}/share/themes/"
cp -rLf $MSYSTEM_PREFIX/share/themes/Fluent2-Light "${TARGETDIR}/share/themes/"
echo "[完成]"
#
read -p '按回车继续...' var