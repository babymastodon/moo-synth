#deprecated

################################
# Download helpers
################################

@add_to_context
def download_and_extract(self, url, dirname, compression='zip'):
    print "Downloading {}".format(url)

    o={'counter':-1}
    def pr(numblocks, block_size, total_size):
        dl_size = numblocks*block_size
        c = 10*dl_size/total_size
        if c > o['counter']:
            o['counter']= c
            print "{} of {} bytes -- {}%".format(dl_size, total_size, 10*c)

    def rm(path):
        if os.path.isdir(path):
            shutil.rmtree(path)
        if os.path.isfile(path) or os.path.islink(path):
            os.remove(path)

    filename, headers = urllib.urlretrieve(url, reporthook=pr)
    out_dir = self.download_dir().make_node(dirname).abspath()
    rm(out_dir)
    if compression=='zip':
        with open(filename) as f:
            z = zipfile.ZipFile(f)
            z.extractall(out_dir)
    elif compression in ['gzip', 'bz2']:
        t = tarfile.open(name=filename, mode='r')
        t.extractall(out_dir)
    else:
        self.fatal("Compression format {} is not supported".format(compression))

    #flatten the extracted stuff if there's only one directory inside
    if len(os.listdir(out_dir))==1:
        inner_dirname = os.listdir(out_dir)[0]
        tmp_dir = self.download_dir().make_node('temp_dir').abspath()
        rm(tmp_dir)
        shutil.move(out_dir, tmp_dir)
        shutil.move(os.path.join(tmp_dir,inner_dirname), out_dir)
        rm(tmp_dir)

@add_to_context
def download_dir(self,suffix=None):
    dd = self.root.find_node(Context.top_dir).find_node('download')
    if suffix:
        dd = dd.make_node(suffix)
    return dd

@add_to_context
def include_dir(self,suffix=None):
    dd = self.root.find_node(Context.top_dir).find_node('include')
    if suffix:
        dd = dd.make_node(suffix)
    return dd

@add_to_context
def include_dirs(self):
    return [self.include_dir(), self.include_dir().get_bld()]


################################
# Download External libraries
################################

def download_stk(self):
    self.download_and_extract('http://ccrma.stanford.edu/software/stk/release/stk-4.4.4.tar.gz',
            'stk',compression='gzip')
    stk_dir = self.download_dir().find_node('stk')


