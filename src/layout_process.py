import optparse
import os
import re
import shlex

def main():
   parser = optparse.OptionParser()
   parser.add_option("--file-list",
                    help="A response file containing the list of icon files to be processed.")  
   parser.add_option("--output",
                    help="The path to output the ID file to.")
                    
   (options, args) = parser.parse_args()

   with open(options.file_list, 'r') as f:
      inputs = shlex.split(f.read())

   outdir = os.path.dirname(options.output)
   target = options.output

   ids = set()
   ids_map = {}
   
   regex = re.compile(r'(?<="@\+id/)(\w+)(?=")')

   for file in inputs:
     print(file)
     with open(file, "rb") as f:
       content = f.read().decode("utf-8")
       result = re.findall(regex, content)
       #ajust whethe have repeat id
       ids.update(result)
   
   ids = sorted(ids)
   
   with open(target, "w") as f:
      f.write("#ifndef _R_ID_H_\n")
      f.write("#define _R_ID_H_\n\n")
      
      f.write("namespace R::id { \n\n")
      index = 0X1A00
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
       content = f.read().decode("utf-8")
       for k,v in ids_map.items():
         content = content.replace("@+id/%s" % k, "%d" % v)
         content = content.replace("@id/%s" % k, "%d" % v)
       
       with open(os.path.join(outdir, os.path.basename(file)), "wb") as out_file:
         out_file.write(content.encode())

if __name__ == '__main__':
  main()
  