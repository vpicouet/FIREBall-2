# from https://askubuntu.com/questions/1028604/backup-clone-live-to-a-new-partition-which-can-be-booted/1028605#1028605

# Update /etc/fstab on clone partition with clone's UUID
echo ""
echo "====================================================================="
echo "Making changes in: $TargetMnt/etc/fstab"
echo "        from UUID: $SourceUUID"
echo "          to UUID: $TargetUUID"
sed -i "s/$SourceUUID/$TargetUUID/g" "$TargetMnt"/etc/fstab

# Update /boot/grub/grub.cfg on clone partition with clone's UUID
echo ""
echo "====================================================================="
echo "Making changes in: $TargetMnt/boot/grub/grub.cfg"
echo "        from UUID: $SourceUUID"
echo "          to UUID: $TargetUUID"
echo "Also change 'quiet splash' to 'nosplash' for environmental awareness"
echo "Suggest first time booting clone you make wallpaper unique"
sed -i "s/$SourceUUID/$TargetUUID/g" "$TargetMnt"/boot/grub/grub.cfg
sed -i "s/quiet splash/nosplash/g" "$TargetMnt"/boot/grub/grub.cfg

# Update grub boot menu
echo ""
echo "====================================================================="
echo "Calling 'update-grub' to create new boot menu"
update-grub

# Unmount and exit

echo ""
echo "====================================================================="
echo "Unmounting $TargetDev as $TargetMnt"

CleanUp             # Remove temporary files

exit 0