class Song(object):
    def __init__(self, lyric):
        self.myLyric = lyric
    
    def sing_me_a_song(self):
        for line in self.myLyric:
            print line

test = Song(["oneSong","twoSongs","threeSongs"])
test.sing_me_a_song()