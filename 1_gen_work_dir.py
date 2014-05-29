import os

def main():
    DEBUG = False
    if not os.path.isdir('1_form'):
        os.mkdir('1_form')

    if not os.path.isdir('2_boxes'):
        os.mkdir('2_boxes')

    if not os.path.isdir('3_cropped'):
        os.mkdir('3_cropped')

    if not os.path.isdir('4_dataset'):
        os.mkdir('4_dataset')

    for i in range(0,10):
        newdirs = ['1_form/'+str(i),\
                    '2_boxes/'+str(i),\
                    '3_cropped/'+str(i)]
                    

        for newdir in newdirs:
            if not os.path.isdir(newdir):
                os.mkdir(newdir)
        

if __name__ == '__main__':
    main()