import os
import pandas as pd


class Highscores():
    
    def __init__(self, score_file):
        """score_file has to be in same folder"""

        self.filepath = os.path.realpath(os.path.join(os.path.dirname(__file__), score_file))
        try:
            self.df = pd.read_csv(self.filepath)
        except:
            self.df = pd.DataFrame(columns=("Name", "Speed", "Accuracy", "Efficiency"))
            self.df.to_csv(self.filepath, index=False)
        self.df.index += 1
        self.df.insert(1, "Total Score", "")

        self.weightings = [1, 1, 1]
        """List: [speed_weighting, accuracy_weighting, efficiency_weighting]"""

        self.set_allTotalScores()
        self.sort_by_total()

    def set_allTotalScores(self):
        self.df["Total Score"] = self.df[["Speed", "Accuracy", "Efficiency"]].apply(lambda row: self.calc_totalScore(*row), axis=1)
        self.sort_by_total()

    def calc_totalScore(self, *row):
        if (sum(self.weightings) == 0):
            totalScore = 0
        else:
            totalScore = sum(s*w for (s, w) in zip(row, self.weightings)) / sum(self.weightings)
        return round(totalScore)

    def add_entry(self, name, *scores):
        new_entry = pd.DataFrame([[name, self.calc_totalScore(*scores), *scores]], columns=self.df.columns)
        self.df = pd.concat([self.df, new_entry], ignore_index=True)
        new_entry.to_csv(self.filepath, mode="a", columns=["Name", "Speed", "Accuracy", "Efficiency"], header=False, index=False)

    def sort_by_total(self):
        self.df.sort_values(by="Total Score", ascending=False, ignore_index=True, inplace=True)
        self.df.index += 1

    

# l_scores = Highscores("linefollowing_scores.csv")
# print(l_scores.df.to_string())