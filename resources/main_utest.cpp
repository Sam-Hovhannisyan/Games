#include "headers/board.hpp"
#include <gtest/gtest.h>

TEST(BoardTest, DefaultConstructor)
{
    SamHovhannisyan::Board::Board<int> board(3, 4);
    EXPECT_EQ(board.getRows(), 3);
    EXPECT_EQ(board.getCols(), 4);
}

TEST(BoardTest, CopyConstructor)
{
    SamHovhannisyan::Board::Board<int> board1(3, 4);
    SamHovhannisyan::Board::Board<int> board2(board1);
    EXPECT_EQ(board2.getRows(), 3);
    EXPECT_EQ(board2.getCols(), 4);
}

TEST(BoardTest, AssignmentOperator)
{
    SamHovhannisyan::Board::Board<int> board1(3, 4);
    SamHovhannisyan::Board::Board<int> board2(2, 2);
    board2 = board1;
    EXPECT_EQ(board2.getRows(), 3);
    EXPECT_EQ(board2.getCols(), 4);
}

TEST(BoardTest, SetValueAndGetValue)
{
    SamHovhannisyan::Board::Board<int> board(3, 4);
    SamHovhannisyan::Coordinate::Coordinate coord(1, 2);
    board.setValue(coord, 5);
    EXPECT_EQ(board(coord), 5);
}

TEST(BoardTest, GetBoard)
{
    SamHovhannisyan::Board::Board<int> board(3, 4);
    std::vector<std::vector<int>> expected = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    EXPECT_EQ(board.getBoard(), expected);
}

TEST(BoardTest, SetRows)
{
    SamHovhannisyan::Board::Board<int> board(3, 4);
    board.setRows(5);
    EXPECT_EQ(board.getRows(), 5);
}

TEST(BoardTest, SetCols)
{
    SamHovhannisyan::Board::Board<int> board(3, 4);
    board.setCols(6);
    EXPECT_EQ(board.getCols(), 6);
}

TEST(BoardTest, OperatorParentheses)
{
    SamHovhannisyan::Board::Board<int> board(3, 4);
    SamHovhannisyan::Coordinate::Coordinate coord(1, 2);
    board(coord) = 10;
    EXPECT_EQ(board(coord), 10);
}

TEST(BoardTest, OperatorEqual)
{
    SamHovhannisyan::Coordinate::Coordinate coord1(1, 2);
    SamHovhannisyan::Coordinate::Coordinate coord2(1, 2);
    EXPECT_TRUE(coord1 == coord2);
}

TEST(BoardTest, OperatorNotEqual)
{
    SamHovhannisyan::Coordinate::Coordinate coord1(1, 2);
    SamHovhannisyan::Coordinate::Coordinate coord2(2, 3);
    EXPECT_TRUE(coord1 != coord2);
}

TEST(BoardTest, OperatorAssignment)
{
    SamHovhannisyan::Coordinate::Coordinate coord1(1, 2);
    SamHovhannisyan::Coordinate::Coordinate coord2;
    coord2 = coord1;
    EXPECT_EQ(coord2.x, 1);
    EXPECT_EQ(coord2.y, 2);
}

TEST(BoardTest, OperatorCopyConstructor)
{
    SamHovhannisyan::Coordinate::Coordinate coord1(1, 2);
    SamHovhannisyan::Coordinate::Coordinate coord2(coord1);
    EXPECT_EQ(coord2.x, 1);
    EXPECT_EQ(coord2.y, 2);
}

TEST(BoardTest, OperatorDefaultConstructor)
{
    SamHovhannisyan::Coordinate::Coordinate coord;
    EXPECT_EQ(coord.x, 0);
    EXPECT_EQ(coord.y, 0);
}

int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
