#ifndef RANGE_BASED_FOR_LOOP_HPP
#define RANGE_BASED_FOR_LOOP_HPP

// NOT BY ME -> http://stackoverflow.com/questions/8572991/how-to-write-the-range-based-for-loop-with-argv

class argv_range
{
public:
    argv_range(int argc, const char * const argv[])
        : argc_(argc), argv_(argv)
    {
    }

    const char * const *begin() const { return argv_; }
    const char * const *end() const { return argv_ + argc_; }

private:
    const int argc_;
    const char * const *argv_;
};

#endif // RANGE_BASED_FOR_LOOP_HPP
