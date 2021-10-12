import argparse
import optparse
import os
import re
import sys
import shlex

def main():
   parser = optparse.OptionParser()
   parser.add_option("--file-list",
                    help="A response file containing the list of icon files "
                         "to be processed.")  
   parser.add_option("--target",
                    help="The path to output the ID file to.")
   parser.add_option("--output-dir",
                    help="The path to xml resource files")  
                    
   (options, args) = parser.parse_args()

   with open(options.file_list, 'r') as f:
      inputs = shlex.split(f.read())

   outdir = options.output_dir
   target_id = os.path.join(outdir, options.target + "_id.h")
   target_filename = options.target + "_resources"
   target_grd = options.target + "_resources.grd"

   ids = set()
   ids_map = {}
   
   regex = re.compile(r'(?<="@\+id/)(\w+)(?=")')

   for file in inputs:
     with open(file, "rb") as f:
       content = f.read()
       result = re.findall(regex, content)
       #ajust whethe have repeat id
       ids.update(result)
   
   ids = sorted(ids)
   
   with open(target_id, "w") as f:
      f.write("#ifndef _R_ID_H_\n")
      f.write("#define _R_ID_H_\n\n")
      
      f.write("namespace R::id { \n\n")
      index = 0X7F0F0000
      for id in ids:
          f.write("constexpr int ")
          strs = id.split('_')

          value=""
          for str in strs:
             value = value + str.title()
          
          f.write("%-25s = 0x%X;\n" % (value,index))
          ids_map[id] = index

          index += 1
      
      #end namespace
      f.write("\n} //namespace R::id\n\n")
      f.write("#endif\n")
  
   for file in inputs:
     with open(file, "rb") as f:
       content = f.read()
       for k,v in ids_map.items():
         content = content.replace("@+id/%s" % k, "@ref/0x%X" % v)
         content = content.replace("@id/%s" % k, "@ref/0x%X" % v)
         #content = content.replace("%s" % k, "@ref/0x%X" % v)
       
       with open(os.path.join(outdir, os.path.basename(file)), "wb") as out_file:
         out_file.write(content)

if __name__ == '__main__':
  main()
  