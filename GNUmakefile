# Nuke built-in rules and variables.
MAKEFLAGS += -rR
.SUFFIXES:

# Default user QEMU flags. These are appended to the QEMU command calls.
QEMUFLAGS := 

override IMAGE_NAME := DoorsOS

# Toolchain for building the 'limine' executable for the host.
HOST_CC := cc
HOST_CFLAGS := -g -O2 -pipe
HOST_CPPFLAGS :=
HOST_LDFLAGS :=
HOST_LIBS :=

.PHONY: all
all: $(IMAGE_NAME).iso

.PHONY: all-hdd
all-hdd: $(IMAGE_NAME).hdd

.PHONY: run
run: $(IMAGE_NAME).iso
	qemu-system-x86_64.exe \
		-cpu qemu64,+sse,+sse2 \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		$(QEMUFLAGS)
.PHONY: run_test_ata
run_test_ata:
	qemu-system-x86_64.exe \
		-cpu qemu64,+sse,+sse2 \
		-cdrom $(IMAGE_NAME).iso \
		-hda disk.img \
		-boot d \
		$(QEMUFLAGS)
.PHONY: make_test_disk
make_test_disk: 
	qemu-img.exe create -f raw disk.img 64M
.PHONY: run-uefi
run-uefi: ovmf/ovmf-code-x86_64.fd $(IMAGE_NAME).iso
	qemu-system-x86_64.exe \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on \
		-cdrom $(IMAGE_NAME).iso \
		-serial stdio \
		$(QEMUFLAGS)

.PHONY: run-hdd
run-hdd: $(IMAGE_NAME).hdd
	qemu-system-x86_64.exe \
		-cpu qemu64,+sse,+sse2 \
		-hda $(IMAGE_NAME).hdd \
		-serial stdio \
		-netdev user,id=n0 \
		-device rtl8139,netdev=n0 \
		-m 1G \
		$(QEMUFLAGS)

.PHONY: run-idk-hdd
run-idk-hdd: $(IMAGE_NAME).hdd disk.img
	qemu-system-x86_64.exe \
		-cpu qemu64,+sse,+sse2 \
		-hda $(IMAGE_NAME).hdd \
		-drive id=disk0,file=disk.img,if=none,format=raw \
		-device ahci,id=ahci0 \
		-device ide-drive,drive=disk0,bus=ahci0.0 \
		-serial stdio \
		-netdev user,id=n0 \
		-device rtl8139,netdev=n0 \
		-m 1G \
		$(QEMUFLAGS)


.PHONY: run-hdd-ahci
run-hdd-ahci: $(IMAGE_NAME).hdd
	clear
	qemu-system-x86_64.exe \
		-drive if=pflash,format=raw,readonly,file=ovmf/ovmf-code-x86_64.fd \
		-drive if=pflash,format=raw,file=ovmf/ovmf-vars-x86_64.fd \
		-drive id=disk,file=$(IMAGE_NAME).hdd,format=raw,if=none \
		-device ahci,id=ahci \
		-device ide-hd,drive=disk,bus=ahci.0 \
		-netdev user,id=n0 \
		-device rtl8139,netdev=n0 \
		-serial stdio \
		-audiodev dsound,id=speaker -machine pcspk-audiodev=speaker \
		-m 2G 



.PHONY: run-hdd-uefi
run-hdd-uefi: ovmf/ovmf-code-x86_64.fd $(IMAGE_NAME).hdd
	qemu-system-x86_64.exe \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on \
		-hda $(IMAGE_NAME).hdd \
		$(QEMUFLAGS)

ovmf/ovmf-code-x86_64.fd:
	mkdir -p ovmf
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-x86_64.fd

limine/limine:
	rm -rf limine
	git clone https://github.com/limine-bootloader/limine.git --branch=v9.x-binary --depth=1
	$(MAKE) -C limine \
		CC="$(HOST_CC)" \
		CFLAGS="$(HOST_CFLAGS)" \
		CPPFLAGS="$(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		LIBS="$(HOST_LIBS)"

kernel-deps:
	./kernel/get-deps
	touch kernel-deps

.PHONY: kernel
kernel: kernel-deps
	$(MAKE) -C kernel

$(IMAGE_NAME).iso: kernel
	rm -rf iso_root
	mkdir -p iso_root/boot
	cp -v kernel/bin/kernel iso_root/boot/
	mkdir -p iso_root/boot/limine
	cp -v limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/boot/limine/
	mkdir -p iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso
	./limine/limine bios-install $(IMAGE_NAME).iso
	rm -rf iso_root

$(IMAGE_NAME).hdd:  kernel
	rm -f DoorsOS.hdd
	@if [ ! -f $(IMAGE_NAME).hdd ]; then \
		dd if=/dev/zero bs=6M count=100 of=$(IMAGE_NAME).hdd; \
		PATH=$$PATH:/usr/sbin:/sbin sgdisk $(IMAGE_NAME).hdd -n 1:2048 -t 1:ef00 -m 1; \
		./limine/limine bios-install $(IMAGE_NAME).hdd; \
		mformat -F -v DOORSOS -i $(IMAGE_NAME).hdd@@1M ::; \
		mmd -i $(IMAGE_NAME).hdd@@1M ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine; \
	fi
	mcopy -i $(IMAGE_NAME).hdd@@1M kernel/bin/kernel ::/boot
	mcopy -i $(IMAGE_NAME).hdd@@1M test.txt ::/
	mcopy -i $(IMAGE_NAME).hdd@@1M test_add ::/
	mcopy -i $(IMAGE_NAME).hdd@@1M test_lib.so ::/
	mcopy -i $(IMAGE_NAME).hdd@@1M capitalism.csv ::/govdb.csv
	mcopy -i $(IMAGE_NAME).hdd@@1M limine.conf limine/limine-bios.sys ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTIA32.EFI ::/EFI/BOOT

.PHONY: clean
clean:
	$(MAKE) -C kernel clean
	rm -rf iso_root $(IMAGE_NAME).iso $(IMAGE_NAME).hdd

.PHONY: distclean
distclean: clean
	$(MAKE) -C kernel distclean
	rm -rf kernel-deps limine ovmf
