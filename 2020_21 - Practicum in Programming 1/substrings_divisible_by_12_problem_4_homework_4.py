"""
    Program returns substrings (not including the full string)
    of given string (representing numbers) which are divisible by 12.
"""


import re  # for checking input


class IvalidInput(Exception):
    pass


class BucketItem:
    # lists of indices of starts and ends of possible substrings
    starts = []  # sorted in ascending order
    ends = []  # sorted in descending


    def __init__(self, starts_, ends_):
        self.starts = starts_
        self.ends = ends_

    def __repr__(self):
        return '{"starts":' + str(self.starts) + ', "ends":' + str(self.ends) + '}'

    def addStart(self, to_add):
        self.starts.append(to_add)

    def addEnd(self, to_add):
        self.ends.append(to_add)

    def getStartEndPairs(self, starts_ends_arr, key):
        """
            Modifies and returns given array. Stores at each start index
            a pair(current bucket's key, upper bound index of ends).
            Upper bound represents that substrings of each start can be formed
            as any combination of (start, end value in ends upto stored index of ends)
            O(N)
        """
        if len(self.ends) == 0 or len(self.starts) == 0:
            return starts_ends_arr

        # keep track of where to start iterating through ends
        end_iter = len(self.ends) - 1
        # go through all starts and find an upper bound idx of ends
        for start in self.starts:
            cur_res = 0
            if end_iter < 0:
                break
            for j in range(end_iter, -1, -1):
                if start < self.ends[j]:
                    cur_res = j + 1
                    end_iter = j
                    break
            starts_ends_arr[start] = (key, cur_res)

        return starts_ends_arr



# RUNNER PROGRAM
def main():
    try:
        number_str = input_number()
    except IvalidInput:
        return

    start_end_bounds, bukcets_ends = getSubstringsIndices(number_str)
    displaySolution(start_end_bounds, bukcets_ends, number_str)
    # bruteForce(number_str)



def getDivisibleBy4(num_str):
    """
        Look for two digit substrings/pairs divisible by 4
        any substring that has such pairs as their endings is then
        divisble by 4.
        Returns last index of each of such pairs in descending order.
    """
    substr_ends = []
    for i in range(len(num_str) - 2, -1, -1):
        cur_sub_number = int(num_str[i : i + 2])
        if not cur_sub_number & 3:  # is divisible by 4 (first 2 binary digits need to be zeros)
            substr_ends.append(i + 1)

    return substr_ends


def getSubstringsIndices(num_str):
    """
        Returns a map(bukcets_ends) with indices of endings of substrings divisible by 12 in given
        string. Also returns an array of following format:
        e.g. arr[3] = (k, n) - all substrings divisible by 12 that start at index 3 in original
        substring can end anywhere between 0th and n-th(n not included) index values
        stored in the k-th bucket, k = -1 if there is no substring divisible by 12 that starts at
        3rd main string's index.
        O(N), N - number of chars in given string
    """
    # substrings consisting of single digit
    # cannot be divisible by 12
    if len(num_str) <= 1:
        return [], {}

    # number is divisible by 12 when it is divisble by 3 and 4
    # number is divisble by 4 if its last 2 digits are divisible by 4
    # number is divisble by 3 if the sum of its digits is divisible by 3


    # store all possible end indices of substrings since ending
    # needs to be divisible by 4 otherwise the number is not divisible by 4
    # therefore neither by 12
    substr_ends = getDivisibleBy4(num_str)
    # print("substr ends: {}".format(substr_ends))

    # Now find all numbers divisible by 3 that end with previously found
    # endings:

    # in order to get start-end pairs of substring the sum of digits in between
    # has to be divisble by 3, since there is aldready a sum of digits from
    # idx 0 to each end-idx then subtract the sums that leave the same remainder
    # when divided by 3 as the end-idx's sum because:
    # a = b - c, b = 3*k + l, c = 3*m + q, l and q are b % 3, c % 3
    # => a = 3 * (k - m) + l - q, and if l = q then 3|a
    # therefore group all start and end indices of same: sum % 3

    # compute sums of digits across the string in order to later check division by 3
    sums = [int(num_str[0])]  # add the first element
    for i in range(1, len(num_str)):
        sums.append(sums[i - 1] + int(num_str[i]))

    # print("sums: {}".format(sums))

    # store indices of elements in buckets with keys being
    # sums[idx]  MOD 3
    buckets = {}
    for i in range(3):
        buckets[i] = BucketItem([], [])

    # store starts into buckets, exclude last el since
    # then 1 char number => not divisible by 12 (accept for 0 which is added artificially later)
    if num_str[0] != '0':
        buckets[0].addStart(0)  # if start is 0th el then sum does not change => sum already had to be divisible by 3
    for i in range(len(sums) - 2):
        if num_str[i + 1] != '0':
            buckets[sums[i] % 3].addStart(i + 1)

    # store ends into buckets:
    for i in range(len(substr_ends)):
        idx = substr_ends[i]
        buckets[sums[idx] % 3].addEnd(idx)

    # print("Buckets starts: {}".format([buckets[key].starts for key in buckets]))
    # print("Buckets ends: {}".format([buckets[key].ends for key in buckets]))

    # form the resulting array pairs(start_index, end_index):
    # nums_positions[start_substr_idx] = (bucket_key, upper_bound_idx_in_ends_of_cur_bucket)
    nums_positions = [(-1, -1) for i in range(len(num_str))]  # preallocate for use of counting sort
    bukcets_ends = {}  # extract all ends from buckets and use to return from function

    # get all pairs for each bucket:
    for key in buckets:
        bukcets_ends[key] = buckets[key].ends
        nums_positions = buckets[key].getStartEndPairs(nums_positions, key)

    # print(nums_positions)
    # print(bukcets_ends)

    return nums_positions, bukcets_ends


def displaySolution(end_idx_bounds, bukcets_ends, num_str):
    """
        Takes in a map(bukcets_ends) with indices of endings of substrings divisible by 12 in original
        string. Also takes in an array (end_idx_bounds) of following format:
        e.g. arr[3] = (k, n) - all substrings divisible by 12 that start at index 3 in original
        substring can end anywhere between 0th and n-th(n not included) index values
        stored in the k-th bucket, k = -1 if there is no substring divisible by 12 that starts at
        3rd main string's index.
        This void logs to stdout all substrings(start-end pairs) divisible by 12
        O(N^3) since O(M * N^2 * N) - M is # of buckets - constant, N^2 is # of pairs, N for printing each substring
    """

    for start in range(len(end_idx_bounds)):
        # log the 0 at given index since no
        # substring can start with 0
        if num_str[start] == '0':
            print("Number {} starting at position {}".format('0', start))
            continue

        key = end_idx_bounds[start][0]
        bound = end_idx_bounds[start][1]

        if key < 0:  # there is no substring divisible by 12 starting at cur start
            continue

        for end_idx in range(bound - 1, -1, -1):
            end = bukcets_ends[key][end_idx]

            # do not output the full(original) string since in task it
            # is not considered a substring
            if start == 0 and end == len(num_str) - 1:
                continue

            # if the substring start with 0 e.g. 024 then skip it since its not a valid number
            # but do make sure to print just the 0 since 0 is divisible by 12, in case
            # of 0024 again print only first 0 since in next iteration a 024 substr will be processed
            print("Number {} starting at position {}".format(num_str[start : end + 1], start))


def input_number():
    """
        Checks if input is a digit only string, in case it is
        that string is returned, otherwise an exception is raised
    """
    is_invalid = re.compile(r"\D")
    raw_string = input('')

    if len(raw_string) == 0:
        raise IvalidInput('empty string')

    if raw_string[0] == '-':
        raw_string = raw_string[1:]

    if is_invalid.search(raw_string) or len(raw_string) == 0:
        raise IvalidInput('String does not contain only numerical values')

    return raw_string



if __name__ == "__main__":
    main()
