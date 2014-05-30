import os
import shutil


DEBUG = False
img_suffix = ['.jpeg','.jpg','.png','.tiff']

def gen_img_unique_file_name(count,total):
    assert(count<=total)
    name = ''
    for i in range(len(str(total)) - len(str(count))):
        name+='0'

    offset = str(count)
    name+=offset

    return name

def copy_img_file(src_path,save_data_path,filename):
    if DEBUG: print src_path,filename
    print src_path,filename
    if not os.path.isdir(save_data_path):
        os.mkdir(save_data_path)
    shutil.copyfile(src_path, os.path.join(save_data_path, filename))



def count_img_under_dir(path):
    count = 0
    for label_paths in os.listdir(path): 
        label_path = os.path.join(path, label_paths) 
        if os.path.isdir(label_path) and label_path[-1] >= '0' and label_path[-1] <= '9' : 
            label = int(label_path[-1])
            assert( label >= 0 and label <=9)
            for digit_img in os.listdir(label_path):
                count+=1
    return count

def gen_label_file(dict,save_label_path):
    label_list = []
    for label in dict.keys():
        times = dict.get(label)
        print 'digit:',label,' has ',times,' imgs'
        label_list+=[label for i in range(times)]
        content = ''
        for label in label_list:
            content += str(label) + '\n'

        with open(save_label_path,'w') as f:
            f.write(content);
            f.close()

    print 'gen_label_file:',save_label_path



def main():
    save_label_path = './4_dataset/testLabel.txt'
    save_data_path = './4_dataset/'
    rootDir ='./3_cropped'
    dict = {} # store num of each digit labels
    total = count_img_under_dir(rootDir)
    uid = 0
    suffix = ""
    print 'total_img:',total
    for label_paths in os.listdir(rootDir): 
        label_path = os.path.join(rootDir, label_paths) 
        if os.path.isdir(label_path) and label_path[-1] >= '0' and label_path[-1] <= '9' : 
            label = int(label_path[-1])
            if DEBUG: print '--------------label:%d--------------'%label
            assert( label >= 0 and label <=9)
            for img_path in os.listdir(label_path):
                if DEBUG: print img_path
                if suffix not in img_suffix:
                    (filepath,filename)=os.path.split(img_path)
                    suffix = os.path.splitext(filename)[-1]
                if suffix in img_suffix:
                    count = dict.get(label)
                    if None == count:
                        dict.update({label:1})
                    else:
                        count += 1
                        dict.update({label:count})
                    uid+=1
                    save_name = gen_img_unique_file_name(uid,total)
                    copy_img_file(os.path.join(label_path,img_path),save_data_path, save_name+suffix )



    print 'database'
    gen_label_file(dict,save_label_path)


if __name__ == '__main__':
    main()