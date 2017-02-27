
pkgname=kernel26-custom
basekernel=2.6.29.1
pkgver=2.6.29.1
pkgrel=0428 # 我用日期来表示
pkgdesc="The Linux Kernel and modules"
arch=('i686')
url="http://www.kernel.org"
depends=('module-init-tools')
provides=(kernel26)
install=kernel26.install

build() {
  # build!
  cd ..
  _kernver="${basekernel}${CONFIG_LOCALVERSION}"
  make || return 1
  mkdir -p $startdir/pkg/{lib/modules,boot}
  make INSTALL_MOD_PATH=$startdir/pkg modules_install || return 1
  cp System.map $startdir/pkg/boot/System.map26-custom
  cp arch/x86/boot/bzImage $startdir/pkg/boot/vmlinuz26-custom
  install -D -m644 .config $startdir/pkg/boot/kconfig26-custom
  # set correct depmod command for install
  sed -i -e "s/KERNEL_VERSION=.*/KERNEL_VERSION=${_kernver}/g" $startdir/kernel26.install
}