#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP
/**
  \brief Represents a "ToDo list" with the changes to be done in the contents of the PIU archive.
**/
class Operations{
public:
    std::vector<std::string> newFiles; // New files to be written into the new PIU archive.
    std::vector<bool> filesKept; /* Files to be written from the old PIU archive to the new one.
                               The position of every boolean value in the vector corresponds
                               to the position of every file in the old PIU archive.
                            */
};

#endif // OPERATIONS_HPP
