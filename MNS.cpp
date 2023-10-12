
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdio>
#include <string>
#include <algorithm>
using namespace cv;
using namespace std;
class MNS
{
public:
	Mat read(string address) {
		Mat image;
		image = imread(address, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);
		return image;
	}
	void show(Mat x) {
		Mat display = x;
		if (display.empty()) {
			cout << "Image File "
				<< "Not Found" << endl;
		}
		else {
			imshow("Display", display);
			waitKey(0);
		}
	}
	Mat DCT_Transformation(Mat input) {
		cv::Mat dct;
		try {
			cv::dct(input, dct);
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}
		return dct;
	}
	Mat Inverse_DCT(Mat dct) {
		cv::Mat idct;
		try {
			cv::idct(dct, idct);
			idct.convertTo(idct, CV_8U);
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}
		return idct;
	}
	vector<int> convert(Mat mat) {
		std::vector<int> array;
		if (mat.isContinuous()) {
			array.assign((float*)mat.datastart, (float*)mat.dataend);
		}
		else {
			for (int i = 0; i < mat.rows; ++i) {
				array.insert(array.end(), mat.ptr<int>(i), mat.ptr<int>(i) + mat.cols * mat.channels());
			}
		}
		return array;

	}
	Mat resize_interpolation(int x, Mat input) {
		Mat output;
		output = Mat::zeros(x, x, CV_32F);
		try
		{
			cv::resize(input, output, cv::Size(x, x), 0, 0);
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}
		return output;
	}
	Mat GCC(Mat input, int x = 8, int choice = 0) {
		int height = x;
		Mat output;
		int top, bottom, left, right;
		int borderType = BORDER_CONSTANT;
		top = (int)(x);
		bottom = top;
		left = top;
		right = top;
		Scalar value(255, 255, 255);
		cv::Size s1 = input.size();
		copyMakeBorder(input, output, top, bottom, left, right, borderType, value);
		cv::Size s2 = output.size();
		try
		{
			if (choice == 0) {
				int input_height = s1.height;
				int input_width = s1.width;
				int padded_height = s2.height;
				int padded_width = s2.width;
				int difference = floor((padded_height - input_height) / 2);
				int middle = floor(padded_width / 2);
				for (int i = 0;i < x;i++) {
					for (int j = middle - 1;j < middle + x - 1;j++) {
						output.at<Vec3b>(i, j)[0] = 0;
						output.at<Vec3b>(i, j)[1] = 0;
						output.at<Vec3b>(i, j)[2] = 0;
					}
				}
				for (int i = middle - 1;i < middle + x - 1;i++) {
					for (int j = 0;j < x;j++) {
						output.at<Vec3b>(i, j)[0] = 0;
						output.at<Vec3b>(i, j)[1] = 0;
						output.at<Vec3b>(i, j)[2] = 0;
					}
				}
				for (int i = padded_height - difference - 1;i < padded_height - 1;i++) {
					for (int j = middle - 1;j < middle + x - 1;j++) {
						output.at<Vec3b>(i, j)[0] = 0;
						output.at<Vec3b>(i, j)[1] = 0;
						output.at<Vec3b>(i, j)[2] = 0;
					}
				}
				for (int i = middle - 1;i < middle + x - 1;i++) {
					for (int j = padded_width - x;j < padded_width;j++) {
						output.at<Vec3b>(i, j)[0] = 255;
						output.at<Vec3b>(i, j)[1] = 255;
						output.at<Vec3b>(i, j)[2] = 255;
					}
				}
			}

		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}
		return output;
	}
	Mat resizer(Mat watermark, Mat cover) {
		cv::Size temp = watermark.size();
		Mat output;
		int new_height, new_width = 0;
		try
		{
			new_height = 8 * temp.height;
			new_width = 8 * temp.width;
			cv::resize(cover, output, cv::Size(new_height, new_width), 0, 0);
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}
		return output;
	}
	Mat binary(Mat x) {
		cv::Size size = x.size();
		Mat output = Mat::zeros(size.height, size.width, CV_8U);
		try

		{
			int count = 0;
			cv::cvtColor(x, output, cv::COLOR_BGR2GRAY);
			cv::threshold(output, output, 128.0, 255.0, THRESH_BINARY);
			for (int k = 0; k < output.rows; k++) {
				for (int j = 0; j < output.cols; j++) {
					if (output.at<uchar>(k, j) == 255) {
						count++;
					}
				}
			}
			//cout << "checker :" << count;
			cout << "Binary + GCC watermark is ready";
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}

		return output;
	}
	Mat color_space_tranform(Mat x) {
		Mat output;
		try
		{
			cv::cvtColor(x, output, cv::COLOR_BGR2YCrCb);
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}
		return output;
	}
	Mat embed(Mat cover_image, Mat water_mark_image, float p = 0) {
		cv::Size ref = cover_image.size();
		Mat process_cover_image = DCT_Transformation(cover_image);
		int index_1 = 0;
		int index_2 = 0;
		int blocks_tot = (int)(ref.width * ref.height / 64);
		int blocks_count = (int)(ref.height / 8);
		for (int i = 0;i < blocks_count;i++) {
			for (int j = 0;j < blocks_count;j++) {
				index_1 = i * 8;
				index_2 = j * 8;
				if (water_mark_image.at<uchar>(i, j) == 255) {
					process_cover_image.at<float>(index_1 + 3, index_2 + 3) = std::max(process_cover_image.at<float>(index_1 + 2, index_2 + 2), process_cover_image.at<float>(index_1 + 2, index_2 + 2) + p);
				}
				else {
					process_cover_image.at<float>(index_1 + 3, index_2 + 3) = std::min(process_cover_image.at<float>(index_1 + 2, index_2 + 2), process_cover_image.at<float>(index_1 + 2, index_2 + 2) - p);
				}
			}
		}

		cout << endl << "process done successfully ----1" << endl;
		Mat output = Inverse_DCT(process_cover_image);
		return output;
	}
	Mat rotate(Mat src, double angle)
	{
		Mat dst;
		cv::Point2f center(src.cols / 2.0, src.rows / 2.0);
		cv::Mat rotation = cv::getRotationMatrix2D(center, angle, 1.0);
		try
		{
			cv::warpAffine(src, dst, rotation, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}
		return dst;
	}

	Mat extract(Mat cover_image, Mat watermark) {
		Mat process, inter_out, inter_out_transform, inter_out_dct, extracted_watermark;
		try
		{
			//cout << endl << "check_1" << endl;
			cv::cvtColor(cover_image, process, cv::COLOR_BGR2GRAY);
			//cout << endl << "check_2" << endl;
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			std::cout << "exception caught: " << err_msg << std::endl;
		}
		cv::Size ref1 = process.size();
		cv::Size ref2 = watermark.size();
		int new_height = 0, new_width = 0;
		new_height = 8 * ref2.height;
		new_width = 8 * ref2.width;
		cv::resize(process, inter_out, cv::Size(new_height, new_width), 0, 0);
		inter_out.convertTo(inter_out_transform, CV_32FC1, 1.0 / 1.0);
		inter_out_dct = DCT_Transformation(inter_out_transform);
		extracted_watermark = Mat(ref2.height, ref2.width, CV_32FC1, 0.0);
		cv::Size ref3 = extracted_watermark.size();
		cout << endl << "Extracted_Watermark" << endl;
		//cout << endl << ref3.height << endl << ref3.width;
		//cout << endl << "Original" << endl;
		//cout << endl << ref2.height << endl << ref2.width;
		int index_1, index_2;
		int blocks_tot = (int)(ref1.width * ref1.height / 64);
		int blocks_count = (int)(ref1.height / 8);
		for (int i = 0;i < blocks_count;i++) {
			for (int j = 0;j < blocks_count;j++) {
				index_1 = i * 8;
				index_2 = j * 8;
				if (inter_out_dct.at<float>(index_1 + 3, index_2 + 3) > inter_out_dct.at<float>(index_1 + 2, index_2 + 2)) {
					extracted_watermark.at<float>(i, j) = 255.0;
				}
				else {
					extracted_watermark.at<float>(i, j) = 0.0;
				}

			}
		}
		return extracted_watermark;

	}
	Mat geo_correction(Mat cover_image, Mat watermark_original, Mat watermark_gcc, int x) {
		cv::Size ref1 = watermark_original.size();
		cv::Size ref2 = watermark_gcc.size();
		Mat inter;
		Mat inter_1;
		inter = GCC(watermark_original, 8, 1);
		inter_1 = binary(inter);
		Mat tbr_cover_image = cover_image;
		Mat temp_2;
		for (int i = 0;i < 360;i++) {
			Mat temp = extract(tbr_cover_image, watermark_gcc);
			Mat s1;
			temp.convertTo(temp, CV_32F);
			watermark_gcc.convertTo(watermark_gcc, CV_32F);
			absdiff(temp, watermark_gcc, s1);
			s1.convertTo(s1, CV_32F);
			s1 = s1.mul(s1);
			Scalar s = sum(s1);
			double mse = s.val[0] / (double)(temp.rows * temp.cols);
			double psnr = 10.0 * log10((255 * 255) / mse);
			if (psnr > 7) {
				cout << endl << "PSNR : " << psnr << endl;
				break;
			}
			else {
				cout << endl << "PSNR : " << psnr << endl;
				if (i < 5) {
					tbr_cover_image = rotate(cover_image, -1 * i);
				}
				else {
					tbr_cover_image = rotate(cover_image, -0.5 * i);
				}
			}
		}


		return tbr_cover_image;

	}
	Mat center_crop(Mat image, int a, int b) {
		Size target_size(256, 256);
		Rect crop_region((image.cols - target_size.width) / 2, (image.rows - target_size.height) / 2, target_size.width, target_size.height);
		Mat cropped_image = image(crop_region);
		return cropped_image;
	}
	Mat crop(Mat image, int type, int x = 0, int y = 0, int height = 672, int width = 672) {
		if (type == 1) {
			Rect roi(x, y, height, width);
			Mat cropped_img = image(roi);
			return cropped_img;
		}
		if (type == 2) {
			int rows = image.rows;
			int cols = image.cols;
			Mat result;
			for (int i = 0;i < (int)(((rows) / 4) + 1);i++) {
				for (int j = 0;j < (int)(((rows) / 4) + 1);j++) {
					image.at<Vec3b>(i, j)[0] = 0;
					image.at<Vec3b>(i, j)[1] = 0;
					image.at<Vec3b>(i, j)[2] = 0;
				}
			}
			result = image;
			return result;
		}
	}
	Mat tranform_crop(Mat cropped_image, Mat original_image) {
		Mat original = original_image;
		Mat cropped = cropped_image;
		Mat newImage = Mat::zeros(original.size(), original.type());
		cropped.copyTo(newImage(Rect(0, 0, cropped.cols, cropped.rows)));
		return newImage;
	}
	Mat salt_and_pepper(Mat image) {
		Mat noisy_image = image.clone();
		srand(time(0));
		int noise_percentage = 5;

		for (int i = 0; i < noisy_image.rows; i++)
		{
			for (int j = 0; j < noisy_image.cols; j++)
			{
				int random_num = rand() % 100;
				if (random_num < noise_percentage / 2)
				{
					noisy_image.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
				}
				else if (random_num > (100 - noise_percentage / 2))
				{
					noisy_image.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
				}
			}
		}
		show(noisy_image);
		return noisy_image;
	}
	Mat sharpen(Mat image) {
		Mat laplacian_image;
		Laplacian(image, laplacian_image, CV_16S, 3);
		convertScaleAbs(laplacian_image, laplacian_image);
		Mat sharpened_image;
		addWeighted(image, 1.5, laplacian_image, -0.5, 0, sharpened_image);
		return laplacian_image;
	}
	Mat scale(Mat image, double scale_factor) {
		Size new_size(image.cols * scale_factor, image.rows * scale_factor);
		Mat scaled_image;
		resize(image, scaled_image, new_size, 0, 0, INTER_LINEAR);
		return scaled_image;
	}
	Mat sliding_window(Mat cropped, Mat original) {
		Mat result;
		int result_cols = original.cols - cropped.cols + 1;
		int result_rows = original.rows - cropped.rows + 1;
		result.create(result_rows, result_cols, CV_32FC1);
		matchTemplate(original, cropped, result, TM_CCORR_NORMED);
		double minVal, maxVal;
		Point minLoc, maxLoc;
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
		Rect roi(maxLoc.x, maxLoc.y, cropped.cols, cropped.rows);
		rectangle(original, roi, Scalar(0, 0, 255), 2);
		Mat output = Mat::zeros(original.rows, original.cols, original.type());
		cropped.copyTo(output(roi));
		return output;
	}
	Mat resizer_scale(Mat cropped, Mat original) {
		cv::Size s_base = original.size();
		Mat output;
		Size new_size(s_base.height, s_base.width);
		resize(original, output, new_size, 0, 0, INTER_LINEAR);
		return output;

	}

};
int main()
{
	MNS process, test, sample;
	Mat y, result, gcc_ouput, cover_transform;
	string cover_path = "";
	string new_path;
	string new_path_crop;
	string new_path_geo;
	string new_path_sca;
	Mat rotated_cover;
	Mat rot_new;
	float angle;
	Mat reference_image;
	Mat checker;
	Mat new_extract_2;
	string embedded_text_2;
	string embedded_text_3;
	Mat final_embedded_image_ycrcb, final_embedded_image_rgb;
	cv::Mat Y;
	Mat final_Y;
	Mat check;
	string embedded_text;
	size_t pos;
	Mat scaled_image;
	vector <Mat> final_channels;
	Mat new_cover;
	Mat extract;
	string embedded_text_1;
	size_t pos_1;
	string new_path_1;
	size_t pos_2;
	string new_path_2;
	string new_path_3;
	Mat scaled_cover;
	Mat cropped_image;
	Mat reference_image_1;
	Mat new_scaled_image;
	Mat scaled_extract;
	string embedded_text_4;
	string embedded_text_5;
	size_t pos_3;
	string new_path_4;
	string new_path_5;
	cv::Size correction_size;
	Mat cropped_cover;
	Mat reference_image_2;
	Mat temp;
	Mat cropped_extract;
	string embedded_text_6;
	Mat cropper;
	int temp_x;
	int temp_y;
	int temp_h;
	int temp_w;
	string embedded_text_7;
	size_t pos_4;
	string new_path_6;
	double factor;
	string new_path_7;
	char path[100];
	cout << "Load the watermark image (Give its path) " << endl;
	cin.getline(path, 100);
	Mat x = process.read(path);
	cv::Size correction = x.size();
	int height_final = correction.height / 12;
	int width_final = correction.width / 12;
	resize(x, x, Size(height_final, width_final), INTER_LINEAR);
	int height = 0;
	int width = 0;
	//process.show(x);
	cout << "Including GCCs in the watermark" << endl;
	Mat new_mat;
	cv::Size s = x.size();
	try
	{
		height = s.height;
		width = s.width;
	}
	catch (cv::Exception& e)
	{
		const char* err_msg = e.what();
		std::cout << "exception caught: " << err_msg << std::endl;
	}
	if (height > width) {
		int remainder = height % 8;
		height = height - remainder;
		new_mat = process.resize_interpolation(height, x);
	}
	else {
		int remainder = width % 8;
		width = width - remainder;
		new_mat = process.resize_interpolation(width, x);
	}
	x = new_mat;
	//GCC's height and width set as 8 by default, one can manipulate it according to the need
	Mat gcc_output = process.GCC(x, 8);
	result = process.binary(gcc_output);
	//sample.show(result);
	string strings = "";
	int choice;
	do {
		cout << "" << endl;
		cout <<endl<< "MENU: Watermark Embedding Tool" << endl;
		cout << "1. Load the cover image" << endl;
		cout << "2. Embed the watermark" << endl;
		cout << "3. Extract the watermark" << endl;
		cout << "4. Correct the cover image (If you think that the image has undergone a geometric attack - requires human visual interospection)" << endl;
		cout << "5. Perform attacks for testing purposes" << endl;
		cout << "6. Exit" << endl;
		cout << "Enter your choice: ";
		cin >> choice;

		switch (choice) {
		case 1:
			cout << "You have selected Option 1" << endl;
			cout << "Load the cover image (Give its path) " << endl;
			std::cin.ignore();
			std::getline(std::cin, cover_path);
			y = process.read(cover_path);
			y = process.resizer(gcc_output, y);
			break;
		case 2:
			cout << "You have selected Option 2" << endl;
			// code for Option 2 goes here
			//cout << cover_path << endl;
			if (y.empty()) {
				cout << "Load cover image first (choose option 1)" << endl;
			}
			else {
				cover_transform = process.color_space_tranform(y);
				//cout << "color transform done"<<endl;
				std::vector<cv::Mat> channels;
				cv::split(cover_transform, channels);
				//cout << "split done"<<endl;
				Y = channels[0];
				Y.convertTo(check, CV_32FC1, 1.0 / 1.0);
				//cout << "conversion done" << endl;
				final_Y = process.embed(check, result, 7);
				//cout << "Embedding done" << endl;
				final_channels.push_back(final_Y);
				final_channels.push_back(channels[1]);
				final_channels.push_back(channels[2]);
				//cout << "color channels done" << endl;
				merge(final_channels, final_embedded_image_ycrcb);
				//cout << "merging done" << endl;
				cv::cvtColor(final_embedded_image_ycrcb, final_embedded_image_rgb, cv::COLOR_YCrCb2BGR);
				//cout << "color conversion done" << endl;
				embedded_text = "_embedded_watermark";
				//cout << cover_path << endl;
				pos = cover_path.find_last_of(".");
				//cout << cover_path << endl;
				if (pos != string::npos) {
					new_path = cover_path.substr(0, pos) + embedded_text + cover_path.substr(pos);
					//cout << new_path << endl;
				}
				else {
					// Handle the case where the period character was not found
					cout << "Not happening";
				}
				//cout << new_path << endl;
				cv::imwrite(new_path, final_embedded_image_rgb);
				cout << endl << "Embedded the cover image. Check the input folder for results." << endl;
				//sample.show(final_embedded_image_rgb);
			}
			break;
		case 3:
			cout << "You have selected Option 3" << endl;
			cout << "Load the cover image from which you want to extract the watermark (Give its path)" << endl;
			std::cin.ignore();
			std::getline(std::cin, new_path);
			std::cout << "Opening file: " << new_path << std::endl;
			new_cover = process.read(new_path);
			extract = process.extract(new_cover, result);
			embedded_text_1 = "_extracted_watermark";
			pos_1 = new_path.find_last_of(".");
			new_path_1 = new_path.substr(0, pos_1) + embedded_text_1 + new_path.substr(pos_1);
			cv::imwrite(new_path_1, extract);
			break;
		case 4:
			cout << "You have selected Option 4" << endl;
			cout << "Correct the corrupted cover image";
			if (y.empty()) {
				cout << "Load the reference cover image first (choose option 1)" << endl;
			}
			else {
				int new_choice;
				do {
					cout << "" << endl;
					cout << "Select the possible geometric attack : " << endl;
					cout << "1. Rotation" << endl;
					cout << "2. Scaling (zoomed in or out)" << endl;
					cout << "3. Cropping" << endl;
					cout << "4. Terminate Correction" << endl;
					cout << "" << endl;
					cin >> new_choice;
					switch (new_choice) {
					case 1:
						cout << "Load the geometrically attacked image (Give its path)" << endl;
						std::cin.ignore();
						std::getline(std::cin, new_path_geo);
						rotated_cover = process.read(new_path_geo);
						reference_image = y;
						checker = process.geo_correction(rotated_cover, new_mat, result, 8);
						new_extract_2 = process.extract(checker, result);
						embedded_text_2 = "_extracted_watermark_from_corrected_image";
						embedded_text_3 = "_corrected_image";
						pos_2 = new_path_geo.find_last_of(".");
						new_path_2 = new_path_geo.substr(0, pos_2) + embedded_text_2 + new_path_geo.substr(pos_2);
						new_path_3 = new_path_geo.substr(0, pos_2) + embedded_text_3 + new_path_geo.substr(pos_2);
						cv::imwrite(new_path_2, new_extract_2);
						cv::imwrite(new_path_3, checker);
						cout << endl << "Rotation Correction done -> Check the results in the folder"<<endl;
						cout <<endl<< "The resultant extracted watermark should match the original watermark in this case (rotation correction), if it does not match then it refers that the corrupted image is not attacked geometrically (rotation) " << endl;
						break;
					case 2:
						cout << "Load the geometrically attacked image (Give its path)" << endl;
						std::cin.ignore();
						std::getline(std::cin, new_path_sca);
						scaled_cover = process.read(new_path_sca);
						reference_image_1 = y;
						new_scaled_image = process.resizer_scale(scaled_cover, reference_image_1);
						scaled_extract = process.extract(new_scaled_image, result);
						embedded_text_4 = "_extracted_watermark_from_corrected_image";
						embedded_text_5 = "_corrected_image";
						pos_3 = new_path_sca.find_last_of(".");
						new_path_4 = new_path_sca.substr(0, pos_3) + embedded_text_4 + new_path_sca.substr(pos_3);
						cout << new_path_4 << endl;
						new_path_5 = new_path_sca.substr(0, pos_3) + embedded_text_5 + new_path_sca.substr(pos_3);
						cout << new_path_5 << endl;
						cv::imwrite(new_path_4, new_scaled_image);
						cv::imwrite(new_path_5, scaled_extract);
						cout << endl << "Check the results in the folder" << endl;
						cout << endl<< "The resultant extracted watermark should match the original watermark in this case (it is robust to scaling), if it does not match then it refers that the corrupted image is not attacked by scaling" << endl;
						break;

					case 3:
						cout << "Load the geometrically attacked image (Give its path)" << endl;
						std::cin.ignore();
						std::getline(std::cin, new_path_crop);;
						cropped_cover = process.read(new_path_crop);
						reference_image_2 = y;
						temp = process.sliding_window(cropped_cover, reference_image_2);
						cropped_extract = process.extract(temp, result);
						embedded_text_6 = "_extracted_watermark_from_corrected_image";
						embedded_text_7 = "_corrected_image";
						pos_4 = new_path_crop.find_last_of(".");
						new_path_6 = new_path_crop.substr(0, pos_4) + embedded_text_6 + new_path_crop.substr(pos_4);
						new_path_7 = new_path_crop.substr(0, pos_4) + embedded_text_7 + new_path_crop.substr(pos_4);
						cv::imwrite(new_path_6, temp);
						cv::imwrite(new_path_7, cropped_extract);
						cout << endl << "Check the results in the folder" << endl;
						cout << endl<< "The resultant extracted watermark should be similar the original watermark in this case, if it does not match then it refers that the corrupted image is not attacked by cropping or it must have undergo severe impact (cropping very small portion)" << endl;
						break;
					case 4:
						cout << "Correction Terminated" << endl;
						break;
					default:
						cout << "Invalid choice. Please try again." << endl;
						break;
					}
				} while (new_choice != 4);
			}
			break;
		case 5:
			cout << "You have selected Option 5" << endl;
			cout << "Perform attacks on the image" << endl;
             	int new_choice;
				do {
					cout << "" << endl;
					cout << "Select the attack that you want to perform : " << endl;
					cout << "1. Rotation" << endl;
					cout << "2. Scaling (zoomed in or out)" << endl;
					cout << "3. Cropping" << endl;
					cout << "4. Terminate Attacks" << endl;
					cout << "" << endl;
					cin >> new_choice;
					switch (new_choice) {
					case 1:
						cout << "Load the image where you wish to perform the rotation attack (Give its path)" << endl;
						std::cin.ignore();
						std::getline(std::cin, new_path_geo);
						rotated_cover = process.read(new_path_geo);
						cout << "" << endl << "Enter the angle (by which you want to rotate the image)" << endl;
						cin >> angle;
						rot_new = process.rotate(rotated_cover, angle);
						embedded_text_3 = "_rotated_";
						pos_2 = new_path_geo.find_last_of(".");
						new_path_3 = new_path_geo.substr(0, pos_2) + embedded_text_3 + new_path_geo.substr(pos_2);
						cv::imwrite(new_path_3, rot_new);
						cout << endl << "Image Rotated -> Check the results in the folder" << endl;
						break;
					case 2:
						cout << "Load the image where you wish to perform the scaling attack (Give its path)" << endl;
						std::cin.ignore();
						std::getline(std::cin, new_path_sca);
						scaled_cover = process.read(new_path_sca);
						cout << "" << endl << "Enter the factor (by which you want to scale the image -> should be greater than 0)" << endl;
						cin >> factor;
						scaled_image = process.scale(scaled_cover, factor);
						embedded_text_5 = "_scaled_image";
						pos_3 = new_path_sca.find_last_of(".");
						new_path_4 = new_path_sca.substr(0, pos_3) + embedded_text_5 + new_path_sca.substr(pos_3);
						cv::imwrite(new_path_4, scaled_image);
						cout << endl << "Check the results in the folder" << endl;
						cout << endl << "Image Scaled -> Check the results in the folder" << endl;
						break;

					case 3:
						cout << "Load the image where you wish to perform the cropping attack (Give its path)" << endl;
						std::cin.ignore();
						std::getline(std::cin, new_path_crop);;
						cropped_cover = process.read(new_path_crop);
						correction_size = cropped_cover.size();
						cout << endl << " Height of loaded image : " << correction_size.height << "  Width of loaded image :  " << correction.width<<endl;
						cout << endl << "You will asked to enter the dimensions so as to do the cropping (Please enter those values such that it should be less than the height and width of the image as displayed above" << endl;
						cout << endl << "Enter x-coordinate from where you want to crop the image (If not sure, enter 0)" << endl;
						cin >> temp_x;
						cout << endl << "Enter y-coordinate from where you want to crop the image (If not sure, enter 0)" << endl;
						cin >> temp_y;
						cout << endl << "Enter height of cropped image as required (Make sure that it is less than (image_height - y)) :" << endl;
						cin >> temp_h;
						cout << endl << "Enter width of cropped image as required (Make sure that it is less than (image_width - x)) :" << endl;
						cin >> temp_w;
						cropped_image = process.crop(cropped_cover, 1, temp_x, temp_y, temp_h, temp_w);
						embedded_text_7 = "_cropped_image";
						pos_4 = new_path_crop.find_last_of(".");
						new_path_7 = new_path_crop.substr(0, pos_4) + embedded_text_7 + new_path_crop.substr(pos_4);
						cv::imwrite(new_path_7, cropped_image);
						cout << endl << "Image Cropped -> Check the results in the folder" << endl;
						break;
					case 4:
						cout << "Correction Terminated" << endl;
						break;
					default:
						cout << "Invalid choice. Please try again." << endl;
						break;
					}
				} while (new_choice != 4);
			break;
		case 6:
			cout << "Program Terminated" << endl;
			break;
		default:
			cout << "Invalid choice. Please try again." << endl;
			break;
		}
	} while (choice != 6);

	return 0;

}