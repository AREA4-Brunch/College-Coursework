"""
    Finds all possible combinations to fill the given
    square matrix so it becomes a magic square.

    Some inputs to try:

        I
            Enter the dimension of the square: 3
            Enter min value in magic square: 12
            Enter max value in magic square: 50
            Enter the matrix representing initial state of magic square.
            Use 0s for empty fields, separate elements by space:
            26 0 28
            0 0 0
            0 29 0
        II
            Enter the dimension of the square: 4
            Enter min value in magic square: 1
            Enter max value in magic square: 35
            Enter the matrix representing initial state of magic square.
            Use 0s for empty fields, separate elements by space:
            30 0 0 7
            0 0 29 0
            0 10 0 0
            0 31 0 9

        III
            Enter the dimension of the square: 4
            Enter min value in magic square: 1
            Enter max value in magic square: 16

            Precalculated magic sum: 34

            Enter the matrix representing initial state of magic square.
            Use 0s for empty fields, separate elements by space:
            7 12 0 0
            0 0 0 0
            0 0 0 0
            0 0 15 4

"""

# Slide 49 at:  https://dms.rs/wp-content/uploads/2020/03/slajdovi_Magichni_kvadrati_VB.pdf


import copy

# for debugging:
# import sys
# from os import path
# PATH_CWD = path.abspath(path.dirname(__file__))


# =================================
# Custom error messages:

class CustomException(Exception):
    message = None
    original_exception_msg = None

    def __init__(self, message_, original_exception_msg_):
        super().__init__(message_)

        self.original_exception_msg = original_exception_msg_
        self.message = message_


    def __str__(self):
        return self.message + ':\n' + self.original_exception_msg + '\n\n'


class MagicSquareError(CustomException):
    message = "Magic Square Error"

    def __init__(self, original_exception_msg_):
        super().__init__(self.message, original_exception_msg_)



# =================================
# Custom Data Structs:

class LinkedListNode:
    # next_ = None
    # data = None

    def __init__(self, data_):
        self.data = data_
        self.next_ = None


# Singly Linked List
class LinkedList:
    # head = None
    # tail = None
    # length = 0

    def __init__(self):
        self.head = None
        self.tail = None
        self.length = 0

    def __str__(self):
        out = ""
        cur = self.head
        while cur:
            out += str(cur.data) + ' '
            cur = cur.next_
        return out

    def appendData(self, data_):
        new_node = LinkedListNode(data_)
        new_node.next_ = None
        if self.length == 0:
            self.head = new_node
        else:
            self.tail.next_ = new_node
        self.tail = new_node
        self.length += 1

    def prependData(self, data_):
        new_node = LinkedListNode(data_)
        new_node.next_ = self.head
        self.head = new_node
        self.length += 1

    def deleteNode(self, node):
        if not node or self.length == 0:
            return

        to_del = node
        if self.length == 1:
            self.head = None
            self.tail = None
        # case of removing the tail
        elif not node.next_:
            self.popBack()
            return
        else:
            node.data = node.next_.data
            node.next_ = node.next_.next_
            to_del = node.next_
            if not node.next_:  # update tail
                self.tail = node
        self.length -= 1
        del to_del

    def popBack(self):
        if self.length == 0:
            return
        if self.length == 1:
            self.tail = None
            del self.head
            self.head = None
        else:
            cur = self.head
            while cur.next_:
                if cur.next_ == self.tail:
                    break
                cur = cur.next_
            cur.next_ = None
            del self.tail
            self.tail = cur

        self.length -= 1

    def popFront(self):
        self.deleteNode(self.head)


class MagicSquareNode:
    # sums by diagonals, rows, columns
    # rows_sums = []  # [row_sum, num_of_missing_vals]
    # cols_sums = []
    # diag1_sum = [0, 0]  # main diagonal - [diag_sum, num_of_missing_vals]
    # diag2_sum = [0, 0]  # off diagonal
    # square = None
    # available_vals = None
    # empty_fields = None  # list of (row, col) pairs of available fields
    # magic_sum = 0  # characteristic sum
    # children = []  #

    # Constructor

    def __init__(self, matrix, available_vals_):
        self.square = copy.deepcopy(matrix)
        self.available_vals = copy.deepcopy(available_vals_)
        self.magic_sum = 0
        self.children = []
        self.empty_fields = None
        self.diag1_sum = [0, 0]
        self.diag2_sum = [0, 0]
        self.rows_sums = []
        self.cols_sums = []
        # calculate row, col and diagonal sums
        # self.calculateCharacteristicSum()

    def __str__(self):
        # return "Sqaure:\n{}\nMagic num:{}\nRows:\n{}\nCols:\n{}\nAvail vals:{}\nEmpty fields:\n{}\nChildren:\n{}\n".format(self.square, self.magic_sum, self.rows_sums,self.cols_sums, self.available_vals, self.empty_fields, self.children)
        for row in self.square:
            for el in row:
                print(el, end=' ')
            print()  # newline for new row

        return ''

    def clearUnnecessaryInfo(self):
        del self.rows_sums
        del self.cols_sums
        del self.diag1_sum
        del self.diag2_sum
        del self.available_vals
        # del self.magic_sum
        # del self.empty_fields

    def setEmptyFields(self, empty_fields_):
        # not deep copy since removing from all - layer by layer ??
        self.empty_fields = copy.deepcopy(empty_fields_)

    # Methods:

    def isPerfectSquare(self):
        return self.isMagicSquare() and self.arePerfectSquareDiagsValid()

    def isMagicSquare(self):
        # in case there are missing fields it is not a magic square
        if self.empty_fields is None or not len(self.empty_fields):
            n = len(self.square)
            for row in range(n):
                for col in range(n):
                    cur = self.square[row][col]
                    if cur == 0:
                        return False

            # in case there are 2 rows/cols/diags with different sums
            # it is not a magic square, otherwise it is since no missing vals
            return self.calculateCharacteristicSum()
        return False

    def arePerfectSquareDiagsValid(self):
        n = len(self.square)
        if n == 1:
            return True

        # store sums on all diags parallel to main diag
        all_diag1_sums = []
        all_diag2_sums = []  # parallel to off diag

        # allocate space for diags
        for i in range(2 * n - 1):  # num of diags is n + n - 1
            all_diag1_sums.append(0)
            all_diag2_sums.append(0)

        for row in range(n):
            for col in range(n):
                all_diag1_sums[row + col] += self.square[row][col]
                all_diag2_sums[row - col + n - 1] += self.square[row][col]

        # check if cross sums are equal to magic sum
        for i in range(n - 1):  # top halves (without main diag)
            if self.magic_sum != all_diag1_sums[i] + all_diag1_sums[i + n]:
                return False

            if self.magic_sum != all_diag2_sums[i] + all_diag2_sums[i + n]:
                return False

        return True

    def makeMove(self, row, col, val):
        self.square[row][col] = val
        # self.calculateCharacteristicSum()
        self.rows_sums[row][0] += val
        self.rows_sums[row][1] -= 1
        self.cols_sums[col][0] += val
        self.cols_sums[col][1] -= 1
        if self.magic_sum <= 0:
            if self.rows_sums[row][1] == 0:
                self.magic_sum = self.rows_sums[row][0]
            if self.cols_sums[col][1] == 0:
                self.magic_sum = self.cols_sums[col][0]
        if row + col == len(self.square) - 1:
            self.diag1_sum[0] += val
            self.diag1_sum[1] -= 1
            if self.diag1_sum[1] == 0:
                self.magic_sum = self.diag1_sum[0]
        if row == col:
            self.diag2_sum[0] += val
            self.diag2_sum[1] -= 1
            if self.diag2_sum[1] == 0:
                self.magic_sum = self.diag2_sum[0]

    def canBecomeMagicSquare(self, row, col, val):
        """
            Returns False if the given move is invalid.
        """
        # no point in trying to fit in more vals
        # if cur sums already differ

        # if there was no characteristic number then adding given
        # value can't make it a non magic square
        if self.magic_sum <= 0:
            return True

        # for squares with central square
        # characteristic sum needs to be divisible by N
        # (since sum of evry row is S)
        if len(self.square) & 1 and self.magic_sum % len(self.square) != 0:
            return False


        # if the value being added completes the row sum
        # check if that sum would ruin the magic
        if self.rows_sums[row][1] == 1 \
        and self.rows_sums[row][0] + val != self.magic_sum:
            return False

        if self.cols_sums[col][1] == 1 \
        and self.cols_sums[col][0] + val != self.magic_sum:
            return False

        # check the same for diagonals
        if row + col == -1 + len(self.square):  # main diagonal
            if self.diag1_sum[1] == 1:  # this move will fill the main diagonal 
                if self.diag1_sum[0] + val != self.magic_sum:
                    return False
                # theorem 8
                if len(self.square) == 3 and self.square[1][1] != 0:
                    if 3 * self.square[1][1] != self.diag1_sum[0] + val:
                        return False
        if row == col:  # off diagonal
            if self.diag2_sum[1] == 1:  # this move will fill the main diagonal 
                if self.diag2_sum[0] + val != self.magic_sum:
                    return False
                # theorem 8
                if len(self.square) == 3 and self.square[1][1] != 0:
                    if 3 * self.square[1][1] != self.diag2_sum[0] + val:
                        return False

        return True

    def calculateCharacteristicSum(self):
        """
            Returns False if the given square has two filled
            rows/cols/diags with different sums.
            Also sets magic_sum (if not already set) to characteristic sum of square
            if there is a row/col/diag without missing els.
        """
        self.calculateSums()

        # if the main diag is full, its sum must be the magic sum
        if self.diag1_sum[1] == 0:
            if self.magic_sum > 0:
                if self.diag1_sum[0] != self.magic_sum:
                    return False
            else:
                self.magic_sum = self.diag1_sum[0]
        if self.diag2_sum[1] == 0:
            if self.magic_sum > 0:
                if self.diag2_sum[0] != self.magic_sum:
                    return False
            else:
                self.magic_sum = self.diag2_sum[0]

        for i in range(len(self.square)):
            if self.rows_sums[i][1] == 0:
                if self.magic_sum > 0:
                    if self.rows_sums[i][0] != self.magic_sum:
                        return False
                else:
                    self.magic_sum = self.rows_sums[i][0]

            if self.cols_sums[i][1] == 0:
                if self.magic_sum > 0:
                    if self.cols_sums[i][0] != self.magic_sum:
                        return False
                else:
                    self.magic_sum = self.cols_sums[i][0]

        return True

    def calculateSums(self):
        """
            Stores sums of each row, col and main and off diagonals.
        """
        n = len(self.square)
        self.rows_sums = []
        self.cols_sums = []
        for i in range(n):
            self.rows_sums.append([0, 0])
            self.cols_sums.append([0, 0])
        self.diag1_sum = [0, 0]
        self.diag2_sum = [0, 0]

        for row in range(n):
            for col in range(n):
                cur = self.square[row][col]
                if cur == 0:  # missing value
                    self.rows_sums[row][1] += 1
                    self.cols_sums[col][1] += 1
                    if row + col == n - 1:
                        self.diag1_sum[1] += 1
                    if row == col:
                        self.diag2_sum[1] += 1
                    continue
                self.rows_sums[row][0] += cur
                self.cols_sums[col][0] += cur
                # in case cur el is on one of diagonals:
                if row + col == n - 1:  # main diagonal
                    self.diag1_sum[0] += cur
                if row == col:  # off diagonal
                    self.diag2_sum[0] += cur


class MagicSquareDecisionTree:
    n = 0  # dimensions of n by n magic square
    limits = None  # range [min_val, max_val] - dtype tuple
    root = None
    is_linear_transformation = False
    # list of (row, col) representing order in which magic square is filled
    filling_order = None

    # Constructor

    def __init__(self, n_, limits_):
        self.n = n_
        self.limits = limits_
        self.is_linear_transformation = False
        self.filling_order = None

        # in case the range of values is [1, n^2] or some
        # constant added/multiplied to it
        precalc_magic_sum = self.precalculateMagicSum(n_, limits_)

        state = []  # matrix representing the square
        # build the initial state:
        print("Enter the matrix representing initial state of magic square.\nUse 0s for empty fields, separate elements by space:")

        used_vals = {}  # mark all values used in the square
        for row in range(n_):
            cur_row = []
            raw_line = input().split(' ')
            for col in range(len(raw_line)):
                cur_row.append(int(raw_line[col]))
                # raise error if given value is out of given range and not flag for
                # empty square (0)
                if cur_row[-1] != 0 and (cur_row[-1] < limits_[0] or cur_row[-1] > limits_[1]):
                    raise MagicSquareError("Given value {} is out of range".format(cur_row[-1]))
                if cur_row[-1] in used_vals.keys() and cur_row[-1] != 0:
                    raise MagicSquareError("Given value {} has been inputted multiple times which is illegal".format(cur_row[-1]))
                if cur_row[-1] != 0:
                    used_vals[cur_row[-1]] = True
            state.append(cur_row)

        # form the linked list of values not yet used in the square
        available_vals = {}
        for i in range(limits_[1], limits_[0] - 1, -1):
            if i not in used_vals.keys():
                # in case value from given range exceeds the sum it should take
                # part in, ignore it (of course if given range disallows negative vals)
                if precalc_magic_sum > 0 and i > precalc_magic_sum and limits_[0] >= 0:
                    continue
                available_vals[i] = True

        # Store state only thing left to add are empty fields
        self.root = MagicSquareNode(state, available_vals)
        self.root.magic_sum = precalc_magic_sum

        # Validate the initial state:
        # (holds only for the linear transformation)
        if n_ == 3 and precalc_magic_sum > 0:
            # middle square needs to be magic_sum / N
            middle = (n_ - 1) // 2
            if self.root.square[middle][middle] != 0 \
            and self.root.square[middle][middle] * n_ != self.root.magic_sum:
                raise MagicSquareError("No magic square can be formed on top of given square state")

        # validate the initial state:
        if not self.root.calculateCharacteristicSum():
            raise MagicSquareError("No magic square can be formed on top of given square state")

    def precalculateMagicSum(self, n, limits):
        """
            Precalculates the magic sum if possible otherwise returns 0
        """
        magic_sum = 0
        n_square = n**2
        # linear transformation of [1, n^2] range
        if n != 1 and (limits[1] - limits[0]) % (n_square - 1) == 0:
            self.is_linear_transformation = True
            # ak + m each el multplied k times and added m to it
            # new magic sum = old magic sum * k + n * m
            k = (limits[1] - limits[0]) // (n_square - 1)
            m = limits[0] - k
            magic_sum = k * ((n * (n_square + 1)) // 2) + n * m

            print("\nPrecalculated magic sum: {}\n".format(magic_sum))

        return magic_sum

    def setFillingOrder(self, node):
        n_ = len(node.square)
        state = node.square
        # store (num_of_non_zero_neighbours, row, col)
        empty_fields_weigthed = LinkedList()
        for row in range(n_):
            for col in range(n_):
                if state[row][col] != 0:
                    continue
                # set as priority by which to order moves to make/fields to fill
                # weight - num of els it is connected to
                weight = 0
                weight += n_ - node.rows_sums[row][1]
                weight += n_ - node.cols_sums[col][1]
                if row + col == n_ - 1:  # main diag:
                    weight += n_ - node.diag1_sum[1]
                if row == col:  # off diag
                    weight += n_ - node.diag2_sum[1]
                # sort in increasing order by neighbours but increasing for coo.
                empty_fields_weigthed.appendData([weight, row, col])
                # empty_fields_weigthed.append((-1 * weight, row, col))
                # empty_fields_weigthed.append((0, row, col))

        # find the move with greatest weight and add it as next move
        # then update all other moves weights and repeat until all
        # moves have been sorted
        empty_fields = []
        # Find first maximum
        cur = empty_fields_weigthed.head
        max_weighted_node = cur
        while cur:
            if cur.data[0] > max_weighted_node.data[0]:
                max_weighted_node = cur
            cur = cur.next_
        if max_weighted_node:
            empty_fields.append((max_weighted_node.data[1], max_weighted_node.data[2]))
            empty_fields_weigthed.deleteNode(max_weighted_node)
        # update all other nodes and find new maximum
        while empty_fields_weigthed.length:
            # update all other nodes:
            cur = empty_fields_weigthed.head
            max_weighted_node = cur
            while cur:
                # if they are on same row then previous
                # move added a connection to cur
                if cur.data[1] == max_weighted_node.data[1]:
                    cur.data[0] += 1  # update weight
                # col
                elif cur.data[2] == max_weighted_node.data[2]:
                    cur.data[0] += 1
                # main diag
                elif cur.data[1] + cur.data[2] == max_weighted_node.data[1] + max_weighted_node.data[2] \
                    and cur.data[1] + cur.data[2] == n_ - 1:
                    cur.data[0] += 1
                # off diag
                elif cur.data[1] == cur.data[2] and max_weighted_node.data[1] == max_weighted_node.data[2]:
                    cur.data[0] += 1
                # update max
                if cur.data[0] > max_weighted_node.data[0]:
                    max_weighted_node = cur
                # update iterator
                cur = cur.next_
            # store (row, col)
            empty_fields.append((max_weighted_node.data[1], max_weighted_node.data[2]))
            empty_fields_weigthed.deleteNode(max_weighted_node)

        self.filling_order = copy.deepcopy(empty_fields)
        # since empty field will be treated as stack in order to
        # save the real order by which magic square will be filled
        # you have to reverse
        if len(empty_fields):
            empty_fields.reverse()

        node.setEmptyFields(empty_fields)

    def __str__(self):
        print("\nOrder in which squares were filled:\n{}\n".format(self.filling_order), end='')

        # add as much white space as needed to make the root
        # in the middle
        queue = LinkedList()  # queue
        lvl_idx = 0
        queue.appendData(['/', self.root])  # (idx_of_parent, node) 
        while queue.length:
            print("\n\nLevel {}:\n".format(lvl_idx))
            num_rows = len(queue.head.data[1].square)  # also num of cols
            lvl_size = queue.length

            # for each node label its parent:
            cur = queue.head
            while cur:
                print("{: >{}s}".format("p:" + str(cur.data[0]), 1 + 3 * num_rows), end="|" + 2*' ')
                cur = cur.next_
            print("\n")  # double new line
            # go through all nodes printing all rows
            # one by one
            for row in range(num_rows):
                cur = queue.head
                while cur:
                    for el in cur.data[1].square[row]:
                        print("{:2d}".format(el), end=' ')
                    # 1 space since above print ends with space
                    print(" | ", end=' ')
                    cur = cur.next_
                print()  # new row
            # exclude from queue cur level and
            # add the new one
            print()  # separate flags by 1 line
            for i in range(lvl_size):
                # store all children / add to next level
                cur_node = queue.head.data[1]
                for child in cur_node.children:
                    queue.appendData([i, child])
                # below the node is idx of node in level or '/'
                # if it has no children
                if not cur_node.children or not len(cur_node.children):
                    print("{: >{}s}".format('/', 1 + 3 * num_rows), end="|" + 2*' ')
                else:
                    # each el takes up 2 digits and is separated by 1 space
                    # => 3 * num_cols
                    print("{: >{}s}".format(str(i) + "th", 1 + 3 * num_rows), end="|" + 2*' ')
                # move on to next node in cur level
                queue.popFront()

            # done with this level
            lvl_idx += 1

        return ''

    def buildMagicSquares(self):
        queue = LinkedList()
        # queue = []

        # linear transformation of [1, 9] range
        if self.n == 3 and self.is_linear_transformation:
            # middle square needs to be magic_sum / N
            # this check has been performed, so insert
            # the value if field is empty
            middle = (self.n - 1) // 2
            if self.root.square[middle][middle] != 0:
                self.setFillingOrder(self.root)
                queue.appendData(self.root)
                # queue.append(self.root)
            else:  # field is empty
                # set val to fill the middle with
                val = self.root.magic_sum // self.n
                # print("Middle val:{}".format(val))
                # add this move as new branch
                self.storeNewState(queue, self.root, middle, middle, val)
                self.setFillingOrder(self.root.children[0])
                self.root.clearUnnecessaryInfo()
                # store new state added pointer already
                # queue.appendData(self.root.children[0])
        else:
            self.setFillingOrder(self.root)
            queue.appendData(self.root)
            # queue.append(self.root)

        # print("Order in which to fill:")
        # print(self.filling_order)

        while queue.length:
        # while len(queue):
            # get current parent/node
            node = queue.head.data
            # node = queue[0]
            # print("Cur in queue(len: {})".format(queue.length))
            # print("Cur in queue(len: {})".format(len(queue)))
            # print("Start queue\n")
            # print(queue)
            # print("end queue\n")
            # print(node)
            queue.deleteNode(queue.head)
            # del queue[0]

            # reached a bottom of full tree
            if not node.empty_fields or len(node.empty_fields) == 0 \
            or not node.available_vals or len(node.available_vals) == 0:
                # print("Done with a branch ({}):".format("Magic Square" if node.isMagicSquare() else "dead end"))
                # print(node)
                print("\n\nFinished building the tree\n")
                return

            # remove invalid moves
            row, col = node.empty_fields.pop()
            for val in node.available_vals.keys():
                # print("Possible move: {} at ({}, {})".format(val, row, col))
                # if this move is invalid just dismiss it
                if not node.canBecomeMagicSquare(row, col, val):
                    # print("Move r:{} c:{} val: {} invalid for state:\n{}".format(row, col, val, node))
                    continue
                # add this move as new branch
                self.storeNewState(queue, node, row, col, val)
                # print("New child:")
                # print(node.children[-1])

            # keep only the square and child nodes and
            # clear all additional data of cur node
            node.clearUnnecessaryInfo()

        print("\n\nFinished building the tree\n")

    def storeNewState(self, queue, parent_node, row, col, val):
        new_square = copy.deepcopy(parent_node)
        new_square.children = []
        # note that this value has been used
        if val in new_square.available_vals.keys():
            del new_square.available_vals[val]
        # add that value to the square
        new_square.makeMove(row, col, val)
        queue.appendData(new_square)
        # queue.append(new_square)
        # store the subtree of node
        parent_node.children.append(new_square)

    def getConditionalPreorder(self, *conditions_args):
        """
            Returns tree as a list formed in preorder. Nodes include in
            preorder are the ones on which all given functions return True.
            Functions are stored in a list.
            If functions are not specfied all the leaves will be included.
        """
        preorder = []
        stack = []
        conditions = list(conditions_args)
        stack.append(self.root)
        while len(stack):
            cur = stack.pop()
            while cur:
                # if no conditions are specified or they both
                # return True inclkude this node into output
                if not conditions \
                or len(conditions) == sum([condition(cur) for condition in conditions]):
                    preorder.append(cur)

                if len(cur.children) == 0:
                    break
                for i in range(len(cur.children) - 1, 0, -1):
                    stack.append(cur.children[i])
                cur = cur.children[0]
        return preorder

    def getLeavesPreorder(self):
        return self.getConditionalPreorder(lambda node: not node.children \
                                            or not len(node.children))

    def displayMagicSquaresPreorder(self):
        is_node_leaf = lambda node: not node.children or not len(node.children)
        is_node_magic_square = lambda node: node.isMagicSquare()
        magic_squares = self.getConditionalPreorder(is_node_leaf, is_node_magic_square)

        # in case nothing is logged notify user its
        # because there were no magic square
        if not len(magic_squares):
            print("There are no magic squares")
            return
        
        print("\nThere are {} magic squares\n".format(len(magic_squares)))
        if len(magic_squares) > 1000:
            return

        for leaf in magic_squares:
            print("{}".format("Magic square" if not leaf.arePerfectSquareDiagsValid() else "Perfect Square"))
            print(leaf)

    def displayPerfectSquaresPreorder(self):
        is_node_leaf = lambda node: not node.children or not len(node.children)
        is_node_perfect_square = lambda node: node.isPerfectSquare()
        perfect_squares = self.getConditionalPreorder(is_node_leaf, is_node_perfect_square)

        # in case nothing is logged notify user its
        # because there were no magic square
        if not len(perfect_squares):
            print("There are no perfect squares")
            return

        for leaf in perfect_squares:
            print("Perfect Square")
            print(leaf)



# =================================
# RUNNER PROGRAM
def main():
    # try:
        n = int( input("Enter the dimension of the square: ") )
        limits = (int(input("Enter min value in magic square: ")), int(input("Enter max value in magic square: ")))
        magic_square = MagicSquareDecisionTree(n, limits)

        # with open(path.join(PATH_CWD, 'logs.txt'), 'w') as sys.stdout:
        magic_square.buildMagicSquares()

        while True:
            displayMenu()
            user_cmd = int(input())

            if user_cmd == 1:  # Display Decision Tree
                print(magic_square)
            elif user_cmd == 2:  # Display Magic Squares in Preorder
                magic_square.displayMagicSquaresPreorder()
            elif user_cmd == 3:  # Display Perfect Magic Square in Preorder
                magic_square.displayPerfectSquaresPreorder()
            else:
                break

    # except Exception as e:
    #     print("\n\nThere was an error :(\n{}\n".format(str(e)))


def displayMenu():
    print("\nPlease choose one of the following\n")
    print("1 Display Decision Tree")
    print("2 Display Magic Squares in Preorder")
    print("3 Display Perfect Magic Square in Preorder")
    print("To Quit enter any other integer\n")


def testForMagic():
    # n = int(input("dimension: "))
    # matr = []
    # for i in range(n):
    #     row = [int(el) for el in input().split(' ')]
    #     matr.append(row)

    # leaf = MagicSquareNode([[2, 7, 6], [9, 5, 1], [4, 3, 8]], {})
    
    leaf = MagicSquareNode([[-3, -2], [0, 2]], {})
    # leaf = MagicSquareNode(matr, {})
    
    # leaf.empty_fields = []
    if leaf.isMagicSquare():
        print(leaf)
        if leaf.arePerfectSquareDiagsValid():
            print("Perfect square")
        else:
            print("Magic square")
    else:
        print("Nothing")


def testQueue():
    queue = LinkedList()
    queue.appendData(7)
    queue.appendData(6)
    print(queue)
    queue.deleteNode(queue.head)
    queue.appendData(12)
    print(queue)


if __name__ == "__main__":
    # testQueue()
    main()
    # testForMagic()
