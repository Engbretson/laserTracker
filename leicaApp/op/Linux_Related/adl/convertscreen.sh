for file in *.adl
do
  /APSshare/caqtdm/caqtdm-4.4.1/caQtDM_Binaries/rhel9-x86_64/adl2ui "$file" 
done

for file in *.adl
do
  /APSshare/bin/phoebus  -main org.csstudio.display.converter.medm.Converter -force   "$file" 
done

mv *.ui ../ui/autoconvert

mv *.bob ../bob/autoconvert

