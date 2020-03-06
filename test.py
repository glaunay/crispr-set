import random
import twobits

def stringGenerator(wLen):
    assert wLen  <= 32 
    return ''.join([random.choice("ATCG") for n in range(wLen)])

def reflex_test():
    for wLen in range(1, 33):
        for iWord in range(100):
            word = stringGenerator(wLen)
            code = twobits.encode(word)
            _word = twobits.decode(code, wLen)
            assert word == _word
            print(f"{word} <=> {code} OK")
'''
def encoder():
    assert twobits.encode("A") == 0
    assert twobits.encode("A") == 0
x = twobits.encode("A")
print(f"??{x}")
twobits.encode("T")
twobits.encode("C")
twobits.encode("G")
twobits.encode("ATCGATCGATCGATCGA")
twobits.encode("AAAAAATTTCCGCGAACGAACGG") 
y=twobits.encode("GCCGTGCTAAGCGTAACAACTTCAAATCCGCG") # = 15543626420740950391
print(f"??{y}")
twobits.decode(15543626420740950391, 32)
z=twobits.encode("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG")
s=twobits.decode(z, 32)
print(f"??{s}")
'''

reflex_test()
print("End of test")
