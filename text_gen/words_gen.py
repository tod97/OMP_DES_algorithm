import random
import string
import os

def words_gen(N, length):
    characters = string.ascii_letters + string.digits + './'

    words = {}
    while len(words) < N:
        new_word = ''.join(random.choice(characters) for _ in range(length))
        words[new_word] = 1
    
    return list(words.keys())

words = words_gen(2000000, 8)

filename = os.path.join(os.path.dirname(__file__), 'words.txt')

with open(filename, 'w') as file:
    if words:
        file.write(words[0])
        for word in words[1:]:
            file.write('\n' + word)
