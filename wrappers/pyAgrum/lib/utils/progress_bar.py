# A Python Library to create a Progress Bar.
# from http://snipperize.todayclose.com/snippet/py/Creating-a-Progress-Bar-in-Console--14251/
# modified by Pierre-Henri Wuillemin (feb 2011)
from __future__ import print_function
import sys

class ProgressBar:
    def __init__(self, title="",min_value = 0, max_value = 100, width=77,**kwargs):
        self.char = kwargs.get('char', '#')
        self.mode = kwargs.get('mode', 'dynamic') # fixed or dynamic
        if not self.mode in ['fixed', 'dynamic']:
            self.mode = 'fixed'

        self.total_width=width

        self.bar = ''
        self.oldvar = ''
        self.hasChanged = True

        self.min = min_value
        self.max = max_value
        self.span = max_value - min_value
        self.amount = 0       # When amount == max, we are 100% done

        self.change_title(title)
        self.update_amount(0)

    def change_title(self,new_title):
        self.title = new_title
        self.width=self.total_width-len(self.title)
        self.build_bar()

    def increment_amount(self, add_amount = 1):
        """
        Increment self.amount by 'add_ammount' or default to incrementing
        by 1, and then rebuild the bar string.
        """
        new_amount = self.amount + add_amount
        if new_amount < self.min: new_amount = self.min
        if new_amount > self.max: new_amount = self.max
        self.amount = new_amount
        self.build_bar()


    def update_amount(self, new_amount = None):
        """
        Update self.amount with 'new_amount', and then rebuild the bar
        string.
        """
        if not new_amount: new_amount = self.amount
        if new_amount < self.min: new_amount = self.min
        if new_amount > self.max: new_amount = self.max
        self.amount = new_amount
        self.build_bar()


    def build_bar(self):
        """
        Figure new percent complete, and rebuild the bar string base on
        self.amount.
        """
        self.oldbar=self.bar

        diff = float(self.amount - self.min)
        percent_done = int(round((diff / float(self.span)) * 100.0))

        # figure the proper number of 'character' make up the bar
        all_full = self.width - 2
        num_hashes = int(round((percent_done * all_full) / 100))

        if self.mode == 'dynamic':
            # build a progress bar with self.char (to create a dynamic bar
            # where the percent string moves along with the bar progress.
            self.bar = self.char * num_hashes
        else:
            # build a progress bar with self.char and spaces (to create a
            # fixe bar (the percent string doesn't move)
            self.bar = self.char * num_hashes + ' ' * (all_full-num_hashes)

        percent_str = str(percent_done) + "%"
        self.bar = self.title+'[ ' + self.bar + ' ] ' + percent_str
        self.hasChanged=(self.oldbar!=self.bar)


    def __str__(self):
        return str(self.bar)

    def display(self):
        """
        Display bar if needed
        """
        if self.hasChanged:
            print('\r', self.bar,end="")
            sys.stdout.flush()
            self.hasChanged=False



if __name__ == '__main__':
    print
    limit = 1000000

    print('Example 1: Fixed Bar')
    prog = ProgressBar("ex1",0, limit, 77, mode='fixed')
    oldprog = str(prog)
    for i in xrange(limit+1):
        prog.update_amount(i)
        if oldprog != str(prog):
            print("\r",prog,end="",flush=True)
            oldprog=str(prog)

    print('\n\n')

    print('Example 2: Dynamic Bar')
    prog = ProgressBar("ex2",0, limit, 77, mode='dynamic', char='-')
    oldprog = str(prog)
    for i in xrange(limit+1):
        prog.increment_amount()
        if oldprog != str(prog):
            print("\r",prog,end="",flush=True)
            oldprog=str(prog)

    print('\n\n')

    print('Example 3: Dynamic Bar')
    prog = ProgressBar("ex3",0, limit, 77, mode='dynamic', char='-')
    oldprog = str(prog)
    for i in xrange(limit+1):
        prog.increment_amount()
        prog.display()

    print('\n\n')

#<code>
#from progress_bar import ProgressBar
#import sys
#
#count = 0
#total = 100000
#
#prog = ProgressBar(count, total, 77, mode='fixed', char='#')
#while count <= total:
#    count += 1
#    prog.increment_amount()
#		 prog.display()
#print
#</code>
