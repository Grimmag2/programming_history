//Authors: Dalibor Kalina
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows.Input;
using ITUvec.Models;
using CommunityToolkit.Mvvm.Input;
using ITUvec.Database;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using ITUvec.Messages;
using GalaSoft.MvvmLight.Messaging;
using System.Windows.Media;
using System.Xml.Linq;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using Microsoft.EntityFrameworkCore.Metadata;
using System.Configuration;

namespace ITUvec.ViewModels
{
    public class ProfileLeftViewModel : BaseViewModel
    {
        private UnitOfWork uof = new UnitOfWork();
        private string _userProfilePicture;
        private string _userNameTextBlock;
        private string _userAgeTextBlock;
        private UserModel _user;
        private string _weightGraphVisibility;
        private string _statsGraphVisibility;
        private string _foodGraphVisibility;
        private string _kcalCheckBox;
        private string _tukyCheckBox;
        private string _sacharidyCheckBox;
        private string _bilkovinyCheckBox;

        private string _kcalColumnWidth;
        private string _tukyColumnWidth;
        private string _sacharidyColumnWidth;
        private string _bilkovinyColumnWidth;

        private string[] _kcalRatio;
        private string[] _tukyRatio;
        private string[] _sacharidyRatio;
        private string[] _bilkovinyRatio;

        private string[] _kcalRatioNegative;
        private string[] _tukyRatioNegative;
        private string[] _sacharidyRatioNegative;
        private string[] _bilkovinyRatioNegative;

        private string[] _datesArray;
        private string[] _datesArraySecond;

        private string _kcalColor;
        private string _tukyColor;
        private string _sacharidyColor;
        private string _bilkovinyColor;

        private int _kcalTotal = 0;
        private int _tukyTotal = 0;
        private int _sacharidyTotal = 0;
        private int _bilkovinyTotal = 0;

        private int recommendedSugars = 0;
        private int recommendedLipids = 0;
        private int recommendedProteins = 0;
        private int recommendedKcal = 0;

        private string[] _firstStroke;
        private string[] _textWeight;
        private string _weightTextBoxText;
        private string _todaysWeightIsGiven;

        private string _kcalCheckBoxBackgroundVisibility;
        private string _tukyCheckBoxBackgroundVisibility;
        private string _sacharidyCheckBoxBackgroundVisibility;
        private string _bilkovinyCheckBoxBackgroundVisibility;

        private string _statsDatesSize;
        private string _daysStreak;

        private string[] _foodColumnWidth;
        private string[] _foodColumnWidthNegative;
        private string[] _foodNames;
        private string[] _foodCount;
        private string[] _foodColor;

        private string _kcalCheckBoxTextColor;
        private string _tukyCheckBoxTextColor;
        private string _sacharidyCheckBoxTextColor;
        private string _bilkovinyCheckBoxTextColor;

        private string[] _textStats;
        private string[] _timeRangeButtonsColor;
        private string _daysStreakFireSize;


        public string[] TimeRangeButtonsColor
        {
            get { return _timeRangeButtonsColor; }
            set { _timeRangeButtonsColor = value; OnPropertyChanged(nameof(TimeRangeButtonsColor)); }
        }
        public string DaysStreakFireSize
        {
            get { return _daysStreakFireSize; }
            set { _daysStreakFireSize = value; OnPropertyChanged(nameof(DaysStreakFireSize)); }
        }

    public string[] TextStats
        {
            get { return _textStats; }
            set { _textStats = value; OnPropertyChanged(nameof(TextStats)); }
        }
        public string[] FoodColumnWidth
        {
            get { return _foodColumnWidth; }
            set { _foodColumnWidth = value; OnPropertyChanged(nameof(FoodColumnWidth)); }
        }
        public string[] FoodColumnWidthNegative
        {
            get { return _foodColumnWidthNegative; }
            set { _foodColumnWidthNegative = value; OnPropertyChanged(nameof(FoodColumnWidthNegative)); }
        }
        public string[] FoodNames
        {
            get { return _foodNames; }
            set { _foodNames = value; OnPropertyChanged(nameof(FoodNames)); }
        }
        public string[] FoodCount
        {
            get { return _foodCount; }
            set { _foodCount = value;OnPropertyChanged(nameof(FoodCount)); }
        }
        public string[] FoodColor
        {
            get { return _foodColor; }
            set { _foodColor = value; OnPropertyChanged(nameof(FoodColor)); }
        }
        public string KcalCheckBoxTextColor
        {
            get { return _kcalCheckBoxTextColor;}
            set { _kcalCheckBoxTextColor = value;OnPropertyChanged(nameof(KcalCheckBoxTextColor));}
        }
        public string TukyCheckBoxTextColor
        {
            get { return _tukyCheckBoxTextColor;}
            set { _tukyCheckBoxTextColor = value; OnPropertyChanged(nameof(TukyCheckBoxTextColor)); }
        }
        public string SacharidyCheckBoxTextColor
        {
            get { return _sacharidyCheckBoxTextColor; }
            set { _sacharidyCheckBoxTextColor = value; OnPropertyChanged(nameof(SacharidyCheckBoxTextColor)); }
        }
        public string BilkovinyCheckBoxTextColor
        {
            get { return _bilkovinyCheckBoxTextColor; }
            set { _bilkovinyCheckBoxTextColor = value; OnPropertyChanged(nameof(BilkovinyCheckBoxTextColor)); }
        }

        public string StatsDatesSize
        {
            get { return _statsDatesSize; }
            set { _statsDatesSize = value; OnPropertyChanged(nameof(StatsDatesSize)); }
        }
        public string DaysStreak
        {
            get { return _daysStreak; }
            set { _daysStreak = value; OnPropertyChanged(nameof(DaysStreak)); }
        }
        
        public string[] FirstStroke
        {
            get { return _firstStroke; }
            set { _firstStroke = value; OnPropertyChanged(nameof(FirstStroke)); }
        }
        public string[] TextWeight
        {
            get { return _textWeight; }
            set { _textWeight = value; OnPropertyChanged(nameof(TextWeight)); }
        }
        public string WeightTextBoxText
        {
            get { return _weightTextBoxText; }
            set { _weightTextBoxText = value; OnPropertyChangedWeight(nameof(WeightTextBoxText)); OnPropertyChanged(nameof(WeightTextBoxText)); }
        }
        public string TodaysWeightIsGiven
        {
            get { return _todaysWeightIsGiven; }
            set { _todaysWeightIsGiven = value; OnPropertyChanged(nameof(TodaysWeightIsGiven)); }
        }

        public int RecommendedKcal
        {
            get { return recommendedKcal; }
            set
            {
                recommendedKcal = value;
            }
        }

        public int RecommendedProteins
        {
            get { return recommendedProteins; }
            set
            {
                recommendedProteins = value;
            }
        }

        public int RecommendedLipids
        {
            get { return recommendedLipids; }
            set
            {
                recommendedLipids = value;
            }
        }

        public int RecommendedSugars
        {
            get { return recommendedSugars; }
            set
            {
                recommendedSugars = value;
            }
        }
        public int KcalTotal
        {
            get { return _kcalTotal; }
            set {_kcalTotal = value; OnPropertyChanged(nameof(KcalTotal));}
        }
        public int TukyTotal
        {
            get { return _tukyTotal; }
            set { _tukyTotal = value; OnPropertyChanged(nameof(TukyTotal)); }
        }
        public int SacharidyTotal
        {
            get { return _sacharidyTotal; }
            set { _sacharidyTotal = value; OnPropertyChanged(nameof(SacharidyTotal)); }
        }
        public int BilkovinyTotal
        {
            get { return _bilkovinyTotal; }
            set { _bilkovinyTotal = value; OnPropertyChanged(nameof(BilkovinyTotal)); }
        }

        public UserModel User
        {
            get { return _user; }
            set { _user = value; OnPropertyChanged(nameof(User)); }
        }
        public string UserProfilePicture
        {
            get { return _userProfilePicture; }
            set { _userProfilePicture = value; OnPropertyChanged(nameof(UserProfilePicture)); }
        }
        public string UserNameTextBlock
        {
            get { return _userNameTextBlock; }
            set { _userNameTextBlock = value; OnPropertyChanged(nameof(UserNameTextBlock)); }
        }
        public string UserAgeTextBlock
        {
            get { return _userAgeTextBlock; }
            set { _userAgeTextBlock = value; OnPropertyChanged(nameof(UserAgeTextBlock)); }
        }
        public string WeightGraphVisibility
        {
            get { return _weightGraphVisibility; }
            set { _weightGraphVisibility = value; OnPropertyChanged(nameof(WeightGraphVisibility)); }
        }
        public string StatsGraphVisibility
        {
            get { return _statsGraphVisibility; }
            set { _statsGraphVisibility = value; OnPropertyChanged(nameof(StatsGraphVisibility)); }
        }
        public string FoodGraphVisibility
        {
            get { return _foodGraphVisibility; }
            set { _foodGraphVisibility = value; OnPropertyChanged(nameof(FoodGraphVisibility)); }
        }
        public string KcalCheckBox
        {
            get { return _kcalCheckBox; }
            set { _kcalCheckBox = value; OnPropertyChangedPLVM(nameof(KcalCheckBox)); }
        }
        public string TukyCheckBox
        {
            get { return _tukyCheckBox; }
            set { _tukyCheckBox = value; OnPropertyChangedPLVM(nameof(TukyCheckBox)); }
        }
        public string SacharidyCheckBox
        {
            get { return _sacharidyCheckBox; }
            set { _sacharidyCheckBox = value; OnPropertyChangedPLVM(nameof(SacharidyCheckBox)); }
        }
        public string BilkovinyCheckBox
        {
            get { return _bilkovinyCheckBox; }
            set { _bilkovinyCheckBox = value; OnPropertyChangedPLVM(nameof(BilkovinyCheckBox)); }
        }
        
        public string KcalColumnWidth
        {
            get { return _kcalColumnWidth; }
            set { _kcalColumnWidth = value; OnPropertyChanged(nameof(KcalColumnWidth)); }
        }
        public string TukyColumnWidth
        {
            get { return _tukyColumnWidth; }
            set { _tukyColumnWidth = value; OnPropertyChanged(nameof(TukyColumnWidth)); }
        }
        public string SacharidyColumnWidth
        {
            get { return _sacharidyColumnWidth; }
            set { _sacharidyColumnWidth = value; OnPropertyChanged(nameof(SacharidyColumnWidth)); }
        }
        public string BilkovinyColumnWidth
        {
            get { return _bilkovinyColumnWidth; }
            set { _bilkovinyColumnWidth = value; OnPropertyChanged(nameof(BilkovinyColumnWidth)); }
        }

        public string KcalColor
        {
            get { return _kcalColor; }
            set { _kcalColor = value; OnPropertyChanged(nameof(KcalColor)); }
        }
        public string TukyColor
        {
            get { return _tukyColor; }
            set { _tukyColor = value; OnPropertyChanged(nameof(TukyColor)); }
        }
        public string SacharidyColor
        {
            get { return _sacharidyColor; }
            set { _sacharidyColor = value;OnPropertyChanged(nameof(SacharidyColor));}
        }
        public string BilkovinyColor
        {
            get { return _bilkovinyColor;}
            set { _bilkovinyColor = value; OnPropertyChanged(nameof(BilkovinyColor)); }
        }

        public string[] KcalRatio
        {
            get { return _kcalRatio; }
            set { _kcalRatio = value; OnPropertyChanged(nameof(KcalRatio)); }
        }
        public string[] TukyRatio
        {
            get { return _tukyRatio; }
            set { _tukyRatio = value; OnPropertyChanged(nameof(TukyRatio)); }
        }
        public string[] SacharidyRatio
        {
            get { return _sacharidyRatio;}
            set { _sacharidyRatio = value;OnPropertyChanged(nameof(SacharidyRatio));}
        }
        public string[] BilkovinyRatio
        {
            get { return _bilkovinyRatio; }
            set { _bilkovinyRatio = value; OnPropertyChanged(nameof(_bilkovinyRatio)); }
        }

        public string[] KcalRatioNegative
        {
            get { return _kcalRatioNegative; }
            set { _kcalRatioNegative = value; OnPropertyChanged(nameof(KcalRatioNegative)); }
        }
        public string[] TukyRatioNegative
        {
            get { return _tukyRatioNegative; }
            set { _tukyRatioNegative = value; OnPropertyChanged(nameof(TukyRatioNegative)); }
        }
        public string[] SacharidyRatioNegative
        {
            get { return _sacharidyRatioNegative; }
            set { _sacharidyRatioNegative = value; OnPropertyChanged(nameof(SacharidyRatioNegative)); }
        }
        public string[] BilkovinyRatioNegative
        {
            get { return _bilkovinyRatioNegative; }
            set { _bilkovinyRatioNegative = value; OnPropertyChanged(nameof(_bilkovinyRatioNegative)); }
        }

        public string[] DatesArray
        {
            get { return _datesArray; }
            set { _datesArray = value; OnPropertyChanged(nameof(DatesArray)); }
        }
        public string[] DatesArraySecond
        {
            get { return _datesArraySecond; }
            set { _datesArraySecond = value; OnPropertyChanged(nameof(DatesArraySecond)); }
        }

        public string KcalCheckBoxBackgroundVisibility
        {
            get { return _kcalCheckBoxBackgroundVisibility; }
            set { _kcalCheckBoxBackgroundVisibility = value; OnPropertyChanged(nameof(KcalCheckBoxBackgroundVisibility)); }
        }
        public string TukyCheckBoxBackgroundVisibility
        {
            get { return _tukyCheckBoxBackgroundVisibility; }
            set { _tukyCheckBoxBackgroundVisibility = value; OnPropertyChanged(TukyCheckBoxBackgroundVisibility); }
        }
        public string SacharidyCheckBoxBackgroundVisibility
        {
            get { return _sacharidyCheckBoxBackgroundVisibility;}
            set { _sacharidyCheckBoxBackgroundVisibility = value;OnPropertyChanged(nameof(SacharidyCheckBoxBackgroundVisibility));}
        }
        public string BilkovinyCheckBoxBackgroundVisibility
        {
            get { return _bilkovinyCheckBoxBackgroundVisibility; }
            set { _bilkovinyCheckBoxBackgroundVisibility = value; OnPropertyChanged(nameof(BilkovinyCheckBoxBackgroundVisibility)); }
        }


        // Function for setting correct values of Bind properties on entering the Profile
        private async Task ReceiveMessageNavigate(NavigateToPageMessage action)
        {
            if (action.Page == "Profile")
            {
                await LoadUserAsync();
                UserProfilePicture = User.Picture.ToString();
                UserNameTextBlock = User.Name.ToString() + " " + User.Surname.ToString();
                UserAgeTextBlock = "Věk: " + User.Age.ToString() + " let";

                RecommendedSugars = User.RecommendedSugar;
                RecommendedProteins = User.RecommendedProteins;
                RecommendedKcal = User.RecommendedCalories;
                RecommendedLipids = User.RecommendedFats;

                WeightGraphVisibility = "Visible";
                StatsGraphVisibility = "Collapsed";
                FoodGraphVisibility = "Collapsed";

                KcalCheckBox = "True";
                TukyCheckBox = "True";
                SacharidyCheckBox = "False";
                BilkovinyCheckBox = "False";

                KcalColor = "#2A4E26";
                TukyColor = "#407439";
                SacharidyColor = "#569B4F";
                BilkovinyColor = "#6DC360";

                StatsDatesSize = "15";
                TimeRangeButtonsColor = new string[6] { "Visible", "Hidden", "Hidden", "Visible", "Hidden", "Visible" };
                
                if (User.Weight != 0)
                    WeightTextBoxText = User.Weight.ToString();

                
    
                await UpdateWeightGraph();
                await UpdateStatsGraph();
                await UpdateFoodGraph();

                await UpdateDaysStreak();
            }
        }

        // Getting User data from database
        protected async Task LoadUserAsync()
        {
            User = await uof.UserRepository.GetUserAsyncWithoutTracking();
        }
        
        // Function called on change of given weight
        protected void OnPropertyChangedWeight([CallerMemberName] string name = null)
        {
            SaveWeightAsync();
        }

        // Function called on change of any of Checkbox value
        protected void OnPropertyChangedPLVM([CallerMemberName] string name = null)
        {
            UpdateWeightGraph();
            UpdateStatsGraph();
            UpdateFoodGraph();
            if (KcalCheckBox == "True")
            {
                KcalColumnWidth = "0.1*";
                KcalCheckBoxBackgroundVisibility = "Visible";
                KcalCheckBoxTextColor = "White";
            }
            else
            {
                KcalColumnWidth = "0";
                KcalCheckBoxBackgroundVisibility = "Hidden";
                KcalCheckBoxTextColor = "#294F27";
            }
            if (TukyCheckBox == "True")
            {
                TukyColumnWidth = "0.1*";
                TukyCheckBoxBackgroundVisibility = "Visible";
                TukyCheckBoxTextColor = "White";
            }
            else
            {
                TukyColumnWidth = "0";
                TukyCheckBoxBackgroundVisibility = "Hidden";
                TukyCheckBoxTextColor = "#294F27";
            }
            if (SacharidyCheckBox == "True")
            {
                SacharidyColumnWidth = "0.1*";
                SacharidyCheckBoxBackgroundVisibility = "Visible";
                SacharidyCheckBoxTextColor = "White";
            }
            else
            {
                SacharidyColumnWidth = "0";
                SacharidyCheckBoxBackgroundVisibility = "Hidden";
                SacharidyCheckBoxTextColor = "#294F27";
            }
            if (BilkovinyCheckBox == "True")
            {
                BilkovinyColumnWidth = "0.1*";
                BilkovinyCheckBoxBackgroundVisibility = "Visible";
                BilkovinyCheckBoxTextColor = "White";
            }
            else
            {
                BilkovinyColumnWidth = "0";
                BilkovinyCheckBoxBackgroundVisibility = "Hidden";
                BilkovinyCheckBoxTextColor = "#294F27";
            }
            
            if (TimeRangeButtonsColor[0] == "Visible") StatsDatesSize = "15"; else StatsDatesSize = "13";
            
        }

        
        // Calculating/Recalculating Favorite foods per week/month/year
        protected async Task UpdateFoodGraph()
        {
            int timeRange = 7;
            if (TimeRangeButtonsColor[0] == "Visible")
                timeRange = 7;
            else if (TimeRangeButtonsColor[2] == "Visible")
                timeRange = 30;
            else if (TimeRangeButtonsColor[4] == "Visible")
                timeRange = 365;

            List<int> foodIdCount = new List<int>();

            for (int i = 0; i<timeRange; i++)
            {
                IEnumerable<MealToFoodModel> temp = await uof.FoodRepository.GetAllMealToFoodsByDateAsyncWithoutTracking(DateTime.Today.AddDays(-i));
                foreach (MealToFoodModel meal in temp)
                {
                    foodIdCount.Add(meal.FoodId);
                }
            }
            

            var foodIdCountGrouped = foodIdCount.GroupBy(i => i);

            var foodIdCountGroupedSorted = foodIdCountGrouped.OrderByDescending(i => i.Count()).ToList();

            int cnt = 0;
            int[] maxFoodIdCount = new int[5];
            foreach (var foodId in foodIdCountGroupedSorted)
            {
                
                FoodModel? potravina = await uof.FoodRepository.GetByIdAsync(foodId.Key);
                if (potravina != null)
                {
                    FoodNames[cnt] = potravina.Name;
                    maxFoodIdCount[cnt] = foodId.Count();
                }
                else
                {
                    FoodNames[cnt] = "";
                    maxFoodIdCount[cnt] = 0;
                }
                cnt++;
                if (cnt >= 5) break;
            }
            

            int roundedUpMaxFoodIdCount = maxFoodIdCount[0] + (10 - maxFoodIdCount[0] % 10);
            if (roundedUpMaxFoodIdCount == 10) roundedUpMaxFoodIdCount = maxFoodIdCount[0] + (5 - maxFoodIdCount[0] % 5);

            for (int i =0; i < 5; i++)
            {
                FoodColumnWidth[i] = maxFoodIdCount[i].ToString() + "*";
                FoodColumnWidthNegative[i] = (roundedUpMaxFoodIdCount - maxFoodIdCount[i]).ToString() + "*";
                FoodCount[i] = maxFoodIdCount[i].ToString();
            }

            string[] colors = new string[] { "#2A4E26", "#407439", "#569B4F", "#6DC360", "#83CF7E"};
            int index = -1;

            for (int i = 0; i < 5; i++)
            {
                if (i == 0)
                {
                    index++;
                    FoodColor[i] = colors[index];
                    
                }
                else if (FoodCount[i] != FoodCount[i - 1])
                {
                    index++;
                    FoodColor[i] = colors[index];
                    
                }
                else if (FoodCount[i] == FoodCount[i- 1])
                {
                    FoodColor[i] = colors[index];
                }
                
            }


            FoodNames = FoodNames;
            FoodColumnWidth = FoodColumnWidth;
            FoodColumnWidthNegative = FoodColumnWidthNegative;
            FoodCount = FoodCount;
            FoodColor = FoodColor;
        }

        // Calculating/Recalculating Bind values for Weight Graph
        protected async Task UpdateWeightGraph()
        {
            int timeRange = 8;
            if (TimeRangeButtonsColor[0] == "Visible")
                timeRange = 8;
            else if (TimeRangeButtonsColor[2] == "Visible")
                timeRange = 32;
            else if (TimeRangeButtonsColor[4] == "Visible")
                timeRange = 371;

            int minWeight = 1000;
            int maxWeight = -1;
            double[] weight = new double[timeRange];

            // Getting all weight in week/month/year. Finding minimum and maximum
            for (int i = 0; i<timeRange; i++)
            {
                DateModel? temp = await uof.DateRepository.GetByDateAsync(Data.CurrentDate.AddDays(-timeRange+1+i));
                if (temp != null)
                {
                    if (temp.Weight != 0)
                    {
                        if (temp.Weight < minWeight) minWeight = temp.Weight;
                        if (temp.Weight > maxWeight) maxWeight = temp.Weight;
                    }
                    weight[i] = (double)temp.Weight;
                }
            }


            minWeight -= minWeight % 5;
            maxWeight += 5-(maxWeight % 5);

            double oneStep = (maxWeight - minWeight)/10.0;

            // Recalculating weight to multiplies of oneStep
            for (int i = 0; i < timeRange; i++)
            {
                if (weight[i] != 0)
                {
                    double modulo = (weight[i]-minWeight) % oneStep;
                    if (modulo < oneStep/2)
                    {
                        weight[i] -= modulo;
                    }
                    else if (modulo >= oneStep/2)
                    {
                        weight[i] += oneStep - modulo;
                    }
                }
            }

            // Setting weight on days without given weight
            for (int i = 0; i < timeRange; i++)
            {
                if (weight[i] == 0)
                {
                    for (int j = i; j<timeRange-1; j++)
                    {
                        if (weight[j] != 0)
                        {
                            weight[i] = weight[j];
                            break;
                        }
                    }
                    if (weight[i] == 0)
                    {
                        for (int j = 0; j < i; j++)
                        {
                            if (weight[j] != 0)
                            {
                                weight[i] = weight[j];
                                break;
                            }
                        }
                    }
                }
            }

            int eigthOfTimeRange = timeRange / 8;
            int cycle = 0;
            int index = 0;
            double[] weightSum = new double[8 + 1]; 
            for (int i =0; i<timeRange; i++)
            {
                
                weightSum[index] += weight[i];
                cycle++;
                if (cycle >= eigthOfTimeRange)
                {
                    weightSum[index] = weightSum[index]/eigthOfTimeRange;
                    cycle = 0;
                    index++;
                }
            }


            for (int i = 0; i < 8; i++)
            {
                if (weightSum[i] != 0)
                {
                    double modulo = (weightSum[i] - minWeight) % oneStep;
                    if (modulo < oneStep / 2)
                    {
                        weightSum[i] -= modulo;
                    }
                    else if (modulo >= oneStep / 2)
                    {
                        weightSum[i] += oneStep - modulo;
                    }
                }
            }



            // Setting correct RowSpan for connecting lines through the graph
            for (int i = 0; i<8; i++)
            {
                for (int j = 0; j < 10; j++)
                {
                    if (weightSum[i] == minWeight + j*oneStep)
                    {
                        FirstStroke[i] = (10 - j).ToString();
                        break;
                    }
                }
            }
            FirstStroke = FirstStroke;
            TextWeight = new string[2] {maxWeight.ToString(), minWeight.ToString()};

            timeRange = 7;
            if (TimeRangeButtonsColor[0] == "Visible")
                timeRange = 7;
            else if (TimeRangeButtonsColor[2] == "Visible")
                timeRange = 28;
            else if (TimeRangeButtonsColor[4] == "Visible")
                timeRange = 364;
            int seventhOfTimeRange = timeRange / 7;

            // Dates Bind
            for (int i = 0; i < timeRange; i+=seventhOfTimeRange)
            {
                DateTime tempDate2 = Data.CurrentDate.AddDays(-timeRange + 1 + i);
                DatesArraySecond[i/seventhOfTimeRange] = tempDate2.Day.ToString() + "." + tempDate2.Month.ToString() + ".";
            }
            DatesArraySecond = DatesArraySecond;


            // Getting weight from user
            DateModel? tempDate = await uof.DateRepository.GetByDateAsync(Data.CurrentDate);
            if (tempDate != null)
            {
                if (tempDate.Weight != 0)
                {
                    WeightTextBoxText = tempDate.Weight.ToString();
                    TodaysWeightIsGiven = "Visible";
                }
                else
                {
                    WeightTextBoxText = "";
                    TodaysWeightIsGiven = "Hidden";
                }
            }
            else
            {
                int tempId = 0;
                IEnumerable<DateModel> allDates = await uof.DateRepository.GetAllAsync();
                foreach (DateModel date in allDates)
                {
                    if (tempId < date.Id)
                        tempId = date.Id + 1;
                }
                DateModel tempModel = new() { Id=tempId,Date=Data.CurrentDate,Weight=0,Liquid=0};

                await uof.DateRepository.InsertAsync(tempModel);
                WeightTextBoxText = "";
                TodaysWeightIsGiven = "Hidden";
            }
        }

        // Calculating Users day streak based od correct kcal, lipids...
        protected async Task UpdateDaysStreak()
        {
            int KcalInPercents;
            int TukyInPercents;
            int SacharidyInPercents;
            int BilkovinyInPercents;
            int days = 0;
            while(true)
            {
                IEnumerable<MealToFoodModel> mtfs = await uof.FoodRepository.GetAllMealToFoodsByDateAsyncWithoutTracking(Data.CurrentDate.AddDays(-days).Date);
                foreach (MealToFoodModel mtf in mtfs)
                {
                    KcalTotal += mtf.Grams / 100 * mtf.Food!.Calories;
                    TukyTotal += mtf.Grams / 100 * mtf.Food!.Fats;
                    SacharidyTotal += mtf.Grams / 100 * mtf.Food!.Sugar;
                    BilkovinyTotal += mtf.Grams / 100 * mtf.Food!.Proteins;
                }
                KcalInPercents = Convert.ToInt32(Convert.ToDouble(KcalTotal) / ((Convert.ToDouble(RecommendedKcal)) / 100));
                TukyInPercents = Convert.ToInt32(Convert.ToDouble(TukyTotal) / ((Convert.ToDouble(RecommendedLipids)) / 100));
                SacharidyInPercents = Convert.ToInt32(Convert.ToDouble(SacharidyTotal) / ((Convert.ToDouble(RecommendedSugars)) / 100));
                BilkovinyInPercents = Convert.ToInt32(Convert.ToDouble(BilkovinyTotal) / ((Convert.ToDouble(RecommendedProteins)) / 100));

                if ((KcalInPercents < 80) || (KcalInPercents > 120)) break;
                if ((TukyInPercents < 80) || (TukyInPercents > 120)) break;
                if ((SacharidyInPercents < 80) || (SacharidyInPercents > 120)) break;
                if ((BilkovinyInPercents < 80) || (BilkovinyInPercents > 120)) break;

                days++;
            }
            DaysStreak = days.ToString() + " dní v řadě";
            DaysStreakFireSize = "0," + (100 - days * 10).ToString() + ",0,0";
        }

        // Calculating/Recalculating graph per week/month/year for kcal, lipids..
        protected async Task UpdateStatsGraph()
        {
            KcalTotal = 0;
            TukyTotal = 0;
            SacharidyTotal = 0;
            BilkovinyTotal = 0;

            int timeRange = 7;
            if (TimeRangeButtonsColor[0] == "Visible")
                timeRange = 7;
            else if (TimeRangeButtonsColor[2] == "Visible")
                timeRange = 28;
            else if (TimeRangeButtonsColor[4] == "Visible")
                timeRange = 364;

            int seventhOfTimeRange = timeRange / 7;
            int index;
            int KcalInPercents;
            int TukyInPercents;
            int SacharidyInPercents;
            int BilkovinyInPercents;

            int realrealMaxPercents = 0;
            int maxPercents = 0;

            for (int i = 0; i < timeRange; i++)
            {
                IEnumerable<MealToFoodModel> mtfs = await uof.FoodRepository.GetAllMealToFoodsByDateAsyncWithoutTracking(Data.CurrentDate.AddDays(-i).Date);
                foreach (MealToFoodModel mtf in mtfs)
                {
                    KcalTotal += mtf.Grams / 100 * mtf.Food!.Calories;
                    TukyTotal += mtf.Grams / 100 * mtf.Food!.Fats;
                    SacharidyTotal += mtf.Grams / 100 * mtf.Food!.Sugar;
                    BilkovinyTotal += mtf.Grams / 100 * mtf.Food!.Proteins;
                }

                if (i % seventhOfTimeRange == seventhOfTimeRange-1)
                {
                    KcalInPercents = Convert.ToInt32(Convert.ToDouble(KcalTotal) / ((Convert.ToDouble(RecommendedKcal)*seventhOfTimeRange) / 100));
                    TukyInPercents = Convert.ToInt32(Convert.ToDouble(TukyTotal) / ((Convert.ToDouble(RecommendedLipids) * seventhOfTimeRange) / 100));
                    SacharidyInPercents = Convert.ToInt32(Convert.ToDouble(SacharidyTotal) / ((Convert.ToDouble(RecommendedSugars) * seventhOfTimeRange) / 100));
                    BilkovinyInPercents = Convert.ToInt32(Convert.ToDouble(BilkovinyTotal) / ((Convert.ToDouble(RecommendedProteins) * seventhOfTimeRange) / 100));

                    index = 6 - (i - (seventhOfTimeRange - 1)) / seventhOfTimeRange;

                    int realMaxPercents = Math.Max(KcalInPercents, Math.Max(TukyInPercents, Math.Max(SacharidyInPercents, BilkovinyInPercents)));
                    realrealMaxPercents = Math.Max(realMaxPercents, realrealMaxPercents);
                    maxPercents = realMaxPercents + (100 - (realMaxPercents % 100));

                    KcalRatio[index] = KcalInPercents.ToString() + "*";
                    KcalRatioNegative[index] = (maxPercents - KcalInPercents).ToString() + "*";

                    TukyRatio[index] = TukyInPercents.ToString() + "*";
                    TukyRatioNegative[index] = (maxPercents - TukyInPercents).ToString() + "*";

                    SacharidyRatio[index] = SacharidyInPercents.ToString() + "*";
                    SacharidyRatioNegative[index] = (maxPercents - SacharidyInPercents).ToString() + "*";

                    BilkovinyRatio[index] = BilkovinyInPercents.ToString() + "*";
                    BilkovinyRatioNegative[index] = (maxPercents - BilkovinyInPercents).ToString() + "*";

                    if (timeRange == 7)
                    {
                        DateTime tempDate = Data.CurrentDate.AddDays(-i);
                        DatesArray[index] = tempDate.Day.ToString() + "." + tempDate.Month.ToString() + ".";
                    }
                    else
                    {
                        DateTime tempDate = Data.CurrentDate.AddDays(-i);
                        DatesArray[index] = tempDate.Day.ToString() + "." + tempDate.Month.ToString() + ".-" + tempDate.AddDays(seventhOfTimeRange-1).Day.ToString() + "." + tempDate.AddDays(seventhOfTimeRange-1).Month.ToString() + ".";
                    }
                    KcalTotal = 0;
                    TukyTotal = 0;
                    SacharidyTotal = 0;
                    BilkovinyTotal = 0;

                }
            }

            KcalRatio = KcalRatio;
            KcalRatioNegative = KcalRatioNegative;
            TukyRatio = TukyRatio;
            TukyRatioNegative = TukyRatioNegative;
            SacharidyRatio = SacharidyRatio;
            SacharidyRatioNegative = SacharidyRatioNegative;
            BilkovinyRatio = BilkovinyRatio;
            BilkovinyRatioNegative = BilkovinyRatioNegative;
            DatesArray = DatesArray;
            int maxmaxPercents = realrealMaxPercents + (100 - (realrealMaxPercents % 100));
            TextStats = new string[2] { maxmaxPercents.ToString() + "%", (maxmaxPercents / 2.0).ToString() + "%" };
        }

        public RelayCommand MoveGraphToLeftCommand { get; set; }
        public RelayCommand MoveGraphToRightCommand { get; set; }
        public RelayCommand WeekRangeCommand { get; set; }
        public RelayCommand MonthRangeCommand { get; set; }
        public RelayCommand YearRangeCommand { get; set; }
        public RelayCommand MinusWeightCommand { get; set; }
        public RelayCommand PlusWeightCommand { get; set; }
        public RelayCommand ChangeWeightCommand { get; set; }

        // Change Graph to the one before
        public void MoveGraphToLeft()
        {
            if (WeightGraphVisibility == "Visible")
            {
                WeightGraphVisibility = "Collapsed";
                FoodGraphVisibility = "Visible";
            }
            else if (FoodGraphVisibility == "Visible")
            {
                FoodGraphVisibility = "Collapsed";
                StatsGraphVisibility = "Visible";
            }
            else if (StatsGraphVisibility == "Visible")
            {
                StatsGraphVisibility = "Collapsed";
                WeightGraphVisibility = "Visible";
            }
        }

        // Change graph to the one after
        public void MoveGraphToRight()
        {
            if (WeightGraphVisibility == "Visible")
            {
                WeightGraphVisibility = "Collapsed";
                StatsGraphVisibility = "Visible";
            }
            else if (FoodGraphVisibility == "Visible")
            {
                FoodGraphVisibility = "Collapsed";
                WeightGraphVisibility = "Visible";
            }
            else if (StatsGraphVisibility == "Visible")
            {
                StatsGraphVisibility = "Collapsed";
                FoodGraphVisibility = "Visible";
            }
        }

        // Updating graphs and visibility (color scheme) of week/month/year buttons on press of week button
        public void WeekRange()
        {
            if (TimeRangeButtonsColor[0] != "Visible")
            {
                TimeRangeButtonsColor = new string[6] { "Visible", "Hidden", "Hidden", "Visible", "Hidden", "Visible" };
                UpdateFoodGraph();
                UpdateWeightGraph();
                UpdateStatsGraph();
            }
        }

        // Updating graphs and visibility (color scheme) of week/month/year buttons on press of month button
        public void MonthRange()
        {
            if (TimeRangeButtonsColor[2] != "Visible")
            {
                TimeRangeButtonsColor = new string[6] { "Hidden", "Visible", "Visible", "Hidden", "Hidden", "Visible" };
                UpdateFoodGraph();
                UpdateWeightGraph();
                UpdateStatsGraph();
            }
        }

        // Updating graphs and visibility (color scheme) of week/month/year buttons on press of year button
        public void YearRange()
        {
            if (TimeRangeButtonsColor[4] != "Visible")
            {
                TimeRangeButtonsColor = new string[6] { "Hidden", "Visible", "Hidden", "Visible", "Visible", "Hidden" };
                UpdateFoodGraph();
                UpdateWeightGraph();
                UpdateStatsGraph();
            }
        }

        // Decreasing todays weight by 1
        public void MinusWeight()
        {
            if (WeightTextBoxText != "")
                WeightTextBoxText = (Int32.Parse(WeightTextBoxText) - 1).ToString();
            UpdateWeightGraph();
        }
        // Increasing todays weight by 1
        public void PlusWeight()
        {
            if (WeightTextBoxText != "")
                WeightTextBoxText = (Int32.Parse(WeightTextBoxText) + 1).ToString();
            UpdateWeightGraph();
        }
        // Update graph on weight change
        public void ChangeWeight()
        {
            if (WeightTextBoxText != "")
                WeightTextBoxText = (Int32.Parse(WeightTextBoxText)).ToString();
            UpdateWeightGraph();
        }

        // Save given weight to database
        protected async Task SaveWeightAsync()
        {
            DateModel? tempDateModel = await uof.DateRepository.GetByDateAsync(Data.CurrentDate);
            if(tempDateModel != null)
            {
                tempDateModel.Weight = Int32.Parse(WeightTextBoxText);
                await uof.SaveAsync();
                TodaysWeightIsGiven = "Visible";
            }
        }

        public ProfileLeftViewModel()
        {
            KcalRatio = new string[7];
            TukyRatio = new string[7];
            SacharidyRatio = new string[7];
            BilkovinyRatio = new string[7];
            KcalRatioNegative = new string[7];
            TukyRatioNegative = new string[7];
            SacharidyRatioNegative = new string[7];
            BilkovinyRatioNegative = new string[7];
            DatesArray = new string[7];
            DatesArraySecond = new string[7];
            FirstStroke = new string[8];
            TextWeight = new string[2];
            FoodColumnWidth= new string[5];
            FoodColumnWidthNegative = new string[5];
            FoodNames = new string[5];
            FoodCount = new string[5];
            FoodColor = new string[5];
            TextStats = new string[2];
            TimeRangeButtonsColor = new string[6];

            Messenger.Default.Register<NavigateToPageMessage>
            (
                 this,
                 async (action) => await ReceiveMessageNavigate(action)
            );

            MoveGraphToLeftCommand = new RelayCommand(MoveGraphToLeft);
            MoveGraphToRightCommand = new RelayCommand(MoveGraphToRight);
            WeekRangeCommand = new RelayCommand(WeekRange);
            MonthRangeCommand = new RelayCommand(MonthRange);
            YearRangeCommand = new RelayCommand(YearRange);
            MinusWeightCommand = new RelayCommand(MinusWeight);
            PlusWeightCommand = new RelayCommand(PlusWeight);
            ChangeWeightCommand = new RelayCommand(ChangeWeight);

            LoadUserAsync();
        }
    }
}
