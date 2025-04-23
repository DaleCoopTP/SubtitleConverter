#include "DynamicArray.h"
#include "Structures.h"
#include "SubtitleFactory.h"
#include "WriteBehavior.h"

#include <gtest/gtest.h>

#include <sstream>

using namespace std;

string getFileExtension(const string &filename)
{
	size_t dotPosition = filename.rfind('.');
	if (dotPosition == string::npos)
	{
		return "";
	}
	return filename.substr(dotPosition);
}

TEST(UtilsTest, ExtensionDetection)
{
	EXPECT_EQ(getFileExtension("video.srt"), ".srt");
	EXPECT_EQ(getFileExtension("track.smi"), ".smi");
	EXPECT_EQ(getFileExtension("movie.ass"), ".ass");
	EXPECT_EQ(getFileExtension("unknownfile"), "");
}

TEST(SubtitleFactoryTest, SRTCreation)
{
	SubtitleFactory factory;
	auto sub = factory.create(".srt");
	ASSERT_NE(sub, nullptr);
	EXPECT_TRUE(dynamic_cast< SRT * >(sub.get()) != nullptr);
}

TEST(SubtitleFactoryTest, SAMICreation)
{
	SubtitleFactory factory;
	auto sub = factory.create(".smi");
	ASSERT_NE(sub, nullptr);
	EXPECT_TRUE(dynamic_cast< SAMI * >(sub.get()) != nullptr);
}

TEST(SubtitleFactoryTest, SSACreation)
{
	SubtitleFactory factory;
	auto sub = factory.create(".ass");
	ASSERT_NE(sub, nullptr);
	EXPECT_TRUE(dynamic_cast< SSA * >(sub.get()) != nullptr);
}

TEST(SubtitleFactoryTest, TTMLCreation)
{
	SubtitleFactory factory;
	auto sub = factory.create(".ttml");
	ASSERT_NE(sub, nullptr);
	EXPECT_TRUE(dynamic_cast< TTML * >(sub.get()) != nullptr);
}

TEST(WriteBehaviorTest, toSRT_WritesCorrectFormat)
{
	toSRT writer;
	DynamicArray< Structures::Node > nodes = { { { 1, 12345, 23456 }, "Hello, world!" } };
	ostringstream out;
	writer.write(out, nodes);

	string expected = "1\n00:00:12,345 --> 00:00:23,456\nHello, world!\n\n";
	EXPECT_EQ(out.str(), expected);
}

TEST(WriteBehaviorTest, toSRT_WritesMultilineSubtitles)
{
	toSRT writer;
	DynamicArray< Structures::Node > nodes = { { { 0, 12345, 23456 }, "First line\nSecond line" } };
	ostringstream out;
	writer.write(out, nodes);

	string expected =
		"1\n"
		"00:00:12,345 --> 00:00:23,456\n"
		"First line\nSecond line\n\n";

	EXPECT_EQ(out.str(), expected);
}

TEST(WriteBehaviorTest, toSAMI_WritesCorrectFormat)
{
	toSAMI writer;
	DynamicArray< Structures::Node > nodes = { { { 2, 5000, 10000 }, "Sample of SAMI" } };
	ostringstream out;
	writer.write(out, nodes);

	string expected =
		"<SAMI>\n"
		"<BODY>\n"
		"<SYNC Start=5000 End=10000>\n"
		"<P>Sample of SAMI</P>\n"
		"</BODY>\n"
		"</SAMI>\n";
	EXPECT_EQ(out.str(), expected);
}

TEST(WriteBehaviorTest, toSSA_WritesCorrectFormat)
{
	toSSA writer;
	DynamicArray< Structures::Node > nodes = { { { 3, 61000, 65000 }, "SSA line" } };
	ostringstream out;
	writer.write(out, nodes);

	string output = out.str();
	EXPECT_TRUE(output.find("[Script Info]") != string::npos);
	EXPECT_TRUE(output.find("[V4+ Styles]") != string::npos);
	EXPECT_TRUE(output.find("[Events]") != string::npos);
	EXPECT_TRUE(output.find("Dialogue: Marked=0,00:01:01.000,00:01:05.000,Default,,0,0,0,SSA line") != string::npos);
}

TEST(WriteBehaviorTest, toTTML_WritesCorrectFormat)
{
	toTTML writer;
	DynamicArray< Structures::Node > nodes = { { { 4, 1234, 5678 }, "TTML line" } };
	ostringstream out;
	writer.write(out, nodes);

	string expected =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<tt xmlns=\"http://www.w3.org/ns/ttml\">\n"
		"<body>\n"
		"<div>\n"
		"<p begin=\"00:00:01.234\" end=\"00:00:05.678\">TTML line</p>\n"
		"</div>\n"
		"</body>\n"
		"</tt>\n";

	EXPECT_EQ(out.str(), expected);
}

TEST(SRTDialogueParseTest, ReturnsInputWhenCapital)
{
	SRT srt;
	string input = "<i>Welcome</i> to the <b>show</b>.";
	string result = srt.dialogueParse(input);
	EXPECT_EQ(result, input);
}

TEST(SRTFileParseTest, ParsesMultilineSubtitle)
{
	std::string srtData =
		"1\n"
		"00:00:01,000 --> 00:00:05,000\n"
		"First line\n"
		"Second line\n"
		"\n";

	std::istringstream iss(srtData);

	SRT srt;
	srt.fileParse(iss);

	ASSERT_EQ(srt.getContents().size(), 1u);

	const Structures::Node &node = srt.getContents()[0];
	EXPECT_EQ(node.time.start, 1000);
	EXPECT_EQ(node.time.end, 5000);
	std::string expectedDialogue = "First line\nSecond line";
	EXPECT_EQ(node.dialogue, expectedDialogue);
}

TEST(SRTFileParseTest, ParsesSingleLineSubtitle)
{
	std::string srtData =
		"1\n"
		"00:00:05,000 --> 00:00:07,000\n"
		"Single line subtitle\n"
		"\n";

	std::istringstream iss(srtData);

	SRT srt;
	srt.fileParse(iss);

	ASSERT_EQ(srt.getContents().size(), 1u);

	const Structures::Node &node = srt.getContents()[0];
	EXPECT_EQ(node.time.start, 5000);
	EXPECT_EQ(node.time.end, 7000);
	EXPECT_EQ(node.dialogue, "Single line subtitle");
}

TEST(SAMIDialogueParseTest, ConvertsBRToNewline)
{
	SAMI sami;
	string input = R"(<P Class=ENUSCC>Subtitle line one.<br>Subtitle line two.</P>)";
	string result = sami.dialogueParse(input);
	string expected = "Subtitle line one.\nSubtitle line two.";
	EXPECT_EQ(result, expected);
}

TEST(SRTGetCollisionsTest, FindsCollidingSubtitles)
{
	SRT srt;
	srt.getContents().clear();
	Structures::Node node1;
	node1.time.start = 1000;
	node1.time.end = 5000;
	node1.dialogue = "A";

	Structures::Node node2;
	node2.time.start = 4000;
	node2.time.end = 6000;
	node2.dialogue = "B";

	Structures::Node node3;
	node3.time.start = 7000;
	node3.time.end = 9000;
	node3.dialogue = "C";

	srt.getContents().push_back(node1);
	srt.getContents().push_back(node2);
	srt.getContents().push_back(node3);

	DynamicArray< Structures::Node > collisions = srt.getCollisions();

	ASSERT_EQ(collisions.size(), 2u);
	EXPECT_EQ(collisions[0].dialogue, "A");
	EXPECT_EQ(collisions[1].dialogue, "B");
}

TEST(SRTSetFormatTest, WrapsDialogueWithFormatting)
{
	SRT srt;
	srt.getContents().clear();

	Structures::Node node;
	node.dialogue = "Hello";
	srt.getContents().push_back(node);

	srt.setFormat();

	ASSERT_EQ(srt.getContents().size(), 1u);
	EXPECT_EQ(srt.getContents()[0].dialogue, "<i>Hello</i>");
}

TEST(SRTDeleteFormatTest, RemovesFormattingTags)
{
	SRT srt;
	srt.getContents().clear();

	Structures::Node node;
	node.dialogue = "<i>Hello</i>";
	srt.getContents().push_back(node);

	srt.deleteFormat();

	ASSERT_EQ(srt.getContents().size(), 1u);
	EXPECT_EQ(srt.getContents()[0].dialogue, "Hello");
}

TEST(SAMIGetCollisionsTest, FindsCollidingSubtitles)
{
	SAMI sami;
	sami.getContents().clear();

	Structures::Node node1;
	node1.time.start = 6000;
	node1.time.end = 6000;
	node1.dialogue = "Node1";

	Structures::Node node2;
	node2.time.start = 5000;
	node2.time.end = 5000;
	node2.dialogue = "Node2";

	Structures::Node node3;
	node3.time.start = 7000;
	node3.time.end = 7000;
	node3.dialogue = "Node3";

	sami.getContents().push_back(node1);
	sami.getContents().push_back(node2);
	sami.getContents().push_back(node3);

	DynamicArray< Structures::Node > collisions = sami.getCollisions();

	ASSERT_EQ(collisions.size(), 2u);
	EXPECT_EQ(collisions[0].dialogue, "Node1");
	EXPECT_EQ(collisions[1].dialogue, "Node2");
}

TEST(SAMISetFormatTest, WrapsDialogueWithFormatting)
{
	SAMI sami;
	sami.getContents().clear();

	Structures::Node node;
	node.dialogue = "Test SAMI";
	sami.getContents().push_back(node);

	sami.setFormat();

	ASSERT_EQ(sami.getContents().size(), 1u);
	EXPECT_EQ(sami.getContents()[0].dialogue, "<i>Test SAMI</i>");
}

TEST(SAMIDeleteFormatTest, RemovesFormattingTags)
{
	SAMI sami;
	sami.getContents().clear();

	Structures::Node node;
	node.dialogue = "<i>Test SAMI</i>";
	sami.getContents().push_back(node);

	sami.deleteFormat();

	ASSERT_EQ(sami.getContents().size(), 1u);
	EXPECT_EQ(sami.getContents()[0].dialogue, "Test SAMI");
}

TEST(SSAGetCollisionsTest, FindsCollidingSubtitles)
{
	SSA ssa;
	ssa.getContents().clear();

	Structures::Node node1;
	node1.time.layer = 1;
	node1.time.start = 1000;
	node1.time.end = 5000;
	node1.dialogue = "A";

	Structures::Node node2;
	node2.time.layer = 1;
	node2.time.start = 4000;
	node2.time.end = 6000;
	node2.dialogue = "B";

	Structures::Node node3;
	node3.time.layer = 2;
	node3.time.start = 2000;
	node3.time.end = 3000;
	node3.dialogue = "C";

	ssa.getContents().push_back(node1);
	ssa.getContents().push_back(node2);
	ssa.getContents().push_back(node3);

	DynamicArray< Structures::Node > collisions = ssa.getCollisions();

	ASSERT_EQ(collisions.size(), 2u);
	EXPECT_EQ(collisions[0].dialogue, "A");
	EXPECT_EQ(collisions[1].dialogue, "B");
}

TEST(SSASetFormatTest, WrapsDialogueWithFormatting)
{
	SSA ssa;
	ssa.getContents().clear();

	Structures::Node node;
	node.dialogue = "Hello SSA";
	ssa.getContents().push_back(node);

	ssa.setFormat();

	ASSERT_EQ(ssa.getContents().size(), 1u);
	EXPECT_EQ(ssa.getContents()[0].dialogue, "{\\b1}Hello SSA{\\b0}");
}

TEST(SSADeleteFormatTest, RemovesFormattingTags)
{
	SSA ssa;
	ssa.getContents().clear();

	Structures::Node node;
	node.dialogue = "{\\b1}Hello SSA{\\b0}";
	ssa.getContents().push_back(node);

	ssa.deleteFormat();

	ASSERT_EQ(ssa.getContents().size(), 1u);
	EXPECT_EQ(ssa.getContents()[0].dialogue, "Hello SSA");
}

TEST(SRTTimeParseTest, ParsesCorrectTime)
{
	SRT srt;
	std::string input = "00:00:01,000 --> 00:00:05,000";
	Structures::Time t = srt.timeParse(input);

	EXPECT_EQ(t.start, 1000);
	EXPECT_EQ(t.end, 5000);
}

TEST(SAMITimeParseTest, ParsesCorrectTime)
{
	SAMI sami;
	std::string input = "<SYNC Start=8500>";
	Structures::Time t = sami.timeParse(input);

	EXPECT_EQ(t.start, 8500);
	EXPECT_EQ(t.end, 8500);
}

TEST(SSATimeParseTest, ParsesCorrectTime)
{
	SSA ssa;
	std::string input = "Dialogue: 1,01:02:03.04,02:03:04.05,Default,,0,0,0,Some dialogue";
	Structures::Time t = ssa.timeParse(input);

	EXPECT_EQ(t.layer, 1);
	EXPECT_EQ(t.start, 3723040);
	EXPECT_EQ(t.end, 7384050);
}

TEST(TTMLTimeParseTest, ParsesCorrectTime)
{
	TTML ttml;
	std::string input = "<p begin=\"01:02:03.040\" end=\"02:03:04.050\">Some text</p>";
	Structures::Time t = ttml.timeParse(input);

	EXPECT_EQ(t.start, 3723040);
	EXPECT_EQ(t.end, 7384050);
}

TEST(TimeTest, TimeConverterConvertsCorrectly)
{
	std::string timeStr = "01:02:03.04";
	int result = Structures::Time::timeConverter(timeStr);
	EXPECT_EQ(result, 3723040);
}

TEST(TimeTest, IsEmptyReturnsTrueIfZero)
{
	Structures::Time t;
	EXPECT_TRUE(t.isEmpty());
}

TEST(TimeTest, IsEmptyReturnsFalseIfNonZero)
{
	Structures::Time t;
	t.start = 1000;
	EXPECT_FALSE(t.isEmpty());
}

TEST(TTMLGetCollisionsTest, FindsCollidingSubtitles)
{
	TTML ttml;
	ttml.getContents().clear();

	Structures::Node node1 = { { 0, 1000, 5000 }, "A" };
	Structures::Node node2 = { { 0, 4000, 6000 }, "B" };
	Structures::Node node3 = { { 0, 7000, 9000 }, "C" };

	ttml.getContents().push_back(node1);
	ttml.getContents().push_back(node2);
	ttml.getContents().push_back(node3);

	DynamicArray< Structures::Node > collisions = ttml.getCollisions();

	ASSERT_EQ(collisions.size(), 2u);
	EXPECT_EQ(collisions[0].dialogue, "A");
	EXPECT_EQ(collisions[1].dialogue, "B");
}

TEST(TTMLSetFormatTest, WrapsDialogueWithFormatting)
{
	TTML ttml;
	ttml.getContents().clear();

	Structures::Node node = { { 0, 0, 0 }, "Hello TTML" };
	ttml.getContents().push_back(node);

	ttml.setFormat();

	ASSERT_EQ(ttml.getContents()[0].dialogue, "<span style=\"italic\">Hello TTML</span>");
}

TEST(TTMLDeleteFormatTest, RemovesFormattingTags)
{
	TTML ttml;
	ttml.getContents().clear();

	Structures::Node node = { { 0, 0, 0 }, "<span style=\"italic\">Hello TTML</span>" };
	ttml.getContents().push_back(node);

	ttml.deleteFormat();

	ASSERT_EQ(ttml.getContents()[0].dialogue, "Hello TTML");
}

TEST(TimeTest, DefaultConstructor)
{
	Structures::Time t;
	EXPECT_EQ(t.layer, 0);
	EXPECT_EQ(t.start, 0);
	EXPECT_EQ(t.end, 0);
	EXPECT_TRUE(t.isEmpty());
}

TEST(TimeTest, ParameterizedConstructor)
{
	Structures::Time t(1, 100, 200);
	EXPECT_EQ(t.layer, 1);
	EXPECT_EQ(t.start, 100);
	EXPECT_EQ(t.end, 200);
	EXPECT_FALSE(t.isEmpty());
}

TEST(TimeTest, TimeConverter)
{
	string timeStr = "01:02:03.04";
	int result = Structures::Time::timeConverter(timeStr);
	EXPECT_EQ(result, 3723040);
}

TEST(NodeTest, DefaultConstructor)
{
	Structures::Node node;
	EXPECT_EQ(node.time.layer, 0);
	EXPECT_EQ(node.time.start, 0);
	EXPECT_EQ(node.time.end, 0);
	EXPECT_EQ(node.dialogue, "");
}

TEST(NodeTest, ParameterizedConstructor)
{
	Structures::Time t(1, 500, 1000);
	string dialogue = "Test dialogue";
	Structures::Node node(t, dialogue);

	EXPECT_EQ(node.time.layer, 1);
	EXPECT_EQ(node.time.start, 500);
	EXPECT_EQ(node.time.end, 1000);
	EXPECT_EQ(node.dialogue, dialogue);
}

TEST(DynamicArrayTest, DefaultConstructor_Size)
{
	DynamicArray< int > arr;
	EXPECT_EQ(arr.size(), 0);
}

TEST(DynamicArrayTest, DefaultConstructor_Capacity)
{
	DynamicArray< int > arr;
	EXPECT_EQ(arr.capacity(), 4);
}

TEST(DynamicArrayTest, InitializerListConstructor_Size)
{
	DynamicArray< int > arr = { 1, 2, 3 };
	EXPECT_EQ(arr.size(), 3);
}

TEST(DynamicArrayTest, InitializerListConstructor_FirstElement)
{
	DynamicArray< int > arr = { 1, 2, 3 };
	EXPECT_EQ(arr[0], 1);
}

TEST(DynamicArrayTest, InitializerListConstructor_LastElement)
{
	DynamicArray< int > arr = { 1, 2, 3 };
	EXPECT_EQ(arr[2], 3);
}

TEST(DynamicArrayTest, PushBack_IncreasesSize)
{
	DynamicArray< int > arr;
	arr.push_back(10);
	EXPECT_EQ(arr.size(), 1);
}

TEST(DynamicArrayTest, PushBack_StoresValue)
{
	DynamicArray< int > arr;
	arr.push_back(20);
	EXPECT_EQ(arr[0], 20);
}

TEST(DynamicArrayTest, PushBack_NoResizeBeforeCapacity)
{
	DynamicArray< int > arr;
	arr.push_back(1);
	arr.push_back(2);
	arr.push_back(3);
	arr.push_back(4);
	EXPECT_EQ(arr.capacity(), 4);
}

TEST(DynamicArrayTest, PushBack_ResizesCapacity)
{
	DynamicArray< int > arr;
	arr.push_back(10);
	arr.push_back(20);
	arr.push_back(30);
	arr.push_back(40);
	arr.push_back(50);
	EXPECT_GT(arr.capacity(), 4);
}

TEST(DynamicArrayTest, OperatorBracket_Read)
{
	DynamicArray< std::string > arr = { "one", "two", "three" };
	EXPECT_EQ(arr[1], "two");
}

TEST(DynamicArrayTest, OperatorBracket_Write)
{
	DynamicArray< std::string > arr = { "one", "two", "three" };
	arr[1] = "changed";
	EXPECT_EQ(arr[1], "changed");
}

TEST(DynamicArrayTest, Clear_ResetsSize)
{
	DynamicArray< int > arr = { 1, 2, 3, 4, 5 };
	arr.clear();
	EXPECT_EQ(arr.size(), 0);
}

TEST(DynamicArrayTest, Clear_IteratorsEqual)
{
	DynamicArray< int > arr = { 1, 2, 3, 4, 5 };
	arr.clear();
	EXPECT_EQ(arr.begin(), arr.end());
}

TEST(DynamicArrayTest, Begin_ReturnsFirstElement)
{
	DynamicArray< int > arr = { 10, 20, 30 };
	EXPECT_EQ(*arr.begin(), arr[0]);
}

TEST(DynamicArrayTest, End_CorrectOffsetFromBegin)
{
	DynamicArray< int > arr = { 10, 20, 30 };
	EXPECT_EQ(arr.end() - arr.begin(), arr.size());
}

TEST(DynamicArrayTest, BeginEnd_IterationSum)
{
	DynamicArray< int > arr = { 100, 200, 300 };
	int sum = 0;
	for (auto it = arr.begin(); it != arr.end(); ++it)
		sum += *it;
	EXPECT_EQ(sum, 600);
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
