#include "winml_tracker/pose_parser.h"

#include <algorithm>
#include <numeric>
#include <functional>
#include <iostream>

const int ROW_COUNT = 13;
const int COL_COUNT = 13;
const int CHANNEL_COUNT = 20;
const int CLASS_COUNT = 20;
using namespace std;
using namespace pose;

bool g_init = false;
std::vector<std::vector<float>> PoseResultsParser::_gridX;
std::vector<std::vector<float>> PoseResultsParser::_gridY;


void PoseResultsParser::initPoseTables()
{
	if (g_init)
	{
		return;
	}
	else
	{
		g_init = true;

		int xCount = 0;
		int yCount = 0;
		float yVal = 0.0f;

		for (int y = 0; y < ROW_COUNT; y++)
		{
			std::vector<float> gridX;
			std::vector<float> gridY;

			for (int x = 0; x <= COL_COUNT; x++) // confirm <= 
			{
				gridX.push_back((float)xCount);
				gridY.push_back(yVal);

				if (yCount++ == COL_COUNT - 1) // confirm col - 1
				{
					yVal += 1.0;
					yCount = 0;
				}

				if (xCount == COL_COUNT - 1)
				{
					xCount = 0;
				}
				else
				{
					xCount++;
				}
			}


			_gridX.push_back(gridX);
			_gridY.push_back(gridY);
		}
	}

}

std::vector<float> operator+(const std::vector<float>& a, const std::vector<float>& b)
{
	std::vector<float> ret;
	for (std::vector<float>::const_iterator aptr = a.begin(); aptr < a.end(); aptr++)
	{
		for (std::vector<float>::const_iterator bptr = b.begin(); bptr < b.end(); bptr++)
		{
			ret.push_back(*aptr + *bptr);
		}
	}

	return ret;
}

Pose PoseResultsParser::GetRecognizedObjects(std::vector<float> modelOutputs, float threshold)
{
	initPoseTables();

//	outputC = outputB.view(1, 19 + num_classes, h*w)
//		print(outputC)
//		output = outputC.transpose(0, 1).contiguous().view(19 + num_classes, h*w)
//		grid_x = torch.linspace(0, w - 1, w).repeat(h, 1).view(h*w)
//		grid_y = torch.linspace(0, h - 1, h).repeat(w, 1).t().contiguous().view(h*w)
//		xs0 = torch.sigmoid(output[0]) + grid_x
//		ys0 = torch.sigmoid(output[1]) + grid_y
//		xs1 = output[2] + grid_x
//		ys1 = output[3] + grid_y


	std::vector<std::vector<float>> output;
	for (int c = 0; c < CLASS_COUNT; c++)
	{
		std::vector<float> chanVec;
		for (int vec = 0; vec < ROW_COUNT * COL_COUNT; vec++)
		{
			chanVec.push_back(modelOutputs[GetOffset(c, vec)]);
		}

		output.push_back(chanVec);
	}

	auto xs0 = Sigmoid(output[0]) + _gridX[0];
	auto ys0 = Sigmoid(output[1]) + _gridY[0];
	auto xs1 = output[2] + _gridX[0];
	auto ys1 = output[3] + _gridY[0];
	auto xs2 = output[4] + _gridX[0];
	auto ys2 = output[5] + _gridY[0];
	auto xs3 = output[6] + _gridX[0];
	auto ys3 = output[7] + _gridY[0];
	auto xs4 = output[8] + _gridX[0];
	auto ys4 = output[9] + _gridY[0];
	auto xs5 = output[10] + _gridX[0];
	auto ys5 = output[11] + _gridY[0];
	auto xs6 = output[12] + _gridX[0];
	auto ys6 = output[13] + _gridY[0];
	auto xs7 = output[14] + _gridX[0];
	auto ys7 = output[15] + _gridY[0];
	auto xs8 = output[16] + _gridX[0];
	auto ys8 = output[17] + _gridY[0];
	auto det_confs = Sigmoid(output[18]);

	float max_conf = -1.0f;
	int max_ind = -1;
	for (int c = 0; c < ROW_COUNT * COL_COUNT; c++)
	{
		float conf = det_confs[c];

		if (conf > max_conf)
		{
			max_conf = conf;
			max_ind = c;
		}
	}

	if (max_ind >= 0)
	{
		Pose pose;
		pose.bounds.push_back({ xs0[max_ind] / (float)COL_COUNT, ys0[max_ind] / (float)ROW_COUNT });
		pose.bounds.push_back({ xs1[max_ind] / (float)COL_COUNT, ys1[max_ind] / (float)ROW_COUNT });
		pose.bounds.push_back({ xs2[max_ind] / (float)COL_COUNT, ys2[max_ind] / (float)ROW_COUNT });
		pose.bounds.push_back({ xs3[max_ind] / (float)COL_COUNT, ys3[max_ind] / (float)ROW_COUNT });
		pose.bounds.push_back({ xs4[max_ind] / (float)COL_COUNT, ys4[max_ind] / (float)ROW_COUNT });
		pose.bounds.push_back({ xs5[max_ind] / (float)COL_COUNT, ys5[max_ind] / (float)ROW_COUNT });
		pose.bounds.push_back({ xs6[max_ind] / (float)COL_COUNT, ys6[max_ind] / (float)ROW_COUNT });
		pose.bounds.push_back({ xs7[max_ind] / (float)COL_COUNT, ys7[max_ind] / (float)ROW_COUNT });
		pose.bounds.push_back({ xs8[max_ind] / (float)COL_COUNT, ys8[max_ind] / (float)ROW_COUNT });
	}

    return Pose();
}

int PoseResultsParser::GetOffset(int x, int y)
{
    //static int channelStride = ROW_COUNT * COL_COUNT;
    //return (channel * channelStride) + (y * COL_COUNT) + x;
	
	// Pose is transposed
	return (x * ROW_COUNT) + y;
}

std::vector<float> PoseResultsParser::Sigmoid(const std::vector<float>& values)
{
	std::vector<float> ret;

	for (std::vector<float>::const_iterator ptr = values.begin(); ptr < values.end(); ptr++)
	{
		float k = (float)std::exp(*ptr);
		ret.push_back(k / (1.0f + k));
	}

	return ret;
}