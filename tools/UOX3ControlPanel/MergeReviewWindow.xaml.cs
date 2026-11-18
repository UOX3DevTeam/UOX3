using System.Windows;
using MessageBox = System.Windows.MessageBox;

namespace UOX3ControlPanel;

public partial class MergeReviewWindow : Window
{
    public string MergedResult => MergedText.Text;

    public MergeReviewWindow(string path, string baseText, string localText, string upstreamText, string mergedText, bool hasConflicts)
    {
        InitializeComponent();
        HeaderText.Text = path;
        BaseText.Text = baseText;
        LocalText.Text = localText;
        UpstreamText.Text = upstreamText;
        MergedText.Text = mergedText;
        StatusText.Text = hasConflicts
            ? "Git found overlapping edits. Resolve every <<<<<<< / ======= / >>>>>>> block before applying."
            : "Git merged these changes automatically. Review the result before applying it.";
    }

    private void Apply_Click(object sender, RoutedEventArgs e)
    {
        if (MergedText.Text.Contains("<<<<<<<") || MergedText.Text.Contains("=======") || MergedText.Text.Contains(">>>>>>>"))
        {
            MessageBox.Show(this, "Resolve all Git conflict markers before applying the merged file.", "Merge is not resolved", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        DialogResult = true;
    }

    private void Cancel_Click(object sender, RoutedEventArgs e) => DialogResult = false;
}
