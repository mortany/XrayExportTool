﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Object_tool
{
    public partial class Settings : Form
    {
        private EditorSettings pSettings = null;
        private Form EditorForm = null;
        private Object_Editor Editor = null;
        public Settings(EditorSettings settings, Form main_form, Object_Editor editor)
        {
            InitializeComponent();
            pSettings = settings;
            EditorForm = main_form;
            Editor = editor;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.FileName = "";
            ofd.Filter = "Xr file|*.xr";

            if (ofd.ShowDialog() == DialogResult.OK)
                GameMtlPath.Text = ofd.FileName;
        }

        private void SaveParams(object sender, FormClosingEventArgs e)
        {
            pSettings.Save(NoCompress);
            pSettings.Save(Debug);
            pSettings.Save(DefaultsCoP);
            pSettings.Save(DefaultsSoC);
            pSettings.Save(MtLoad);
            pSettings.Save(GameMtlPath);
            pSettings.Save(ProgressiveMeshes);
            pSettings.Save(StripifyMeshes);
            pSettings.Save(OptimizeSurfaces);
            pSettings.Save(SoCInfluence);
            pSettings.Save(SplitNormalsChbx);
            pSettings.Save(BuildInMotionsExport);
            pSettings.Save(SmoothSoC);
            pSettings.Save(SmoothCoP);
            pSettings.Save(Anims8Bit);
            pSettings.Save(Anims16Bit);
            pSettings.Save(AnimsNoCompress);
            pSettings.Save(NoCompressLinkLabel);
            pSettings.Save(HQGeometry);
            pSettings.Save(HQGeometryPlus);
            pSettings.Save(ScaleCenterOfMassCheckBox);
        }

        private void Settings_Load(object sender, EventArgs e)
        {
            pSettings.Load(NoCompress);
            pSettings.Load(Debug);
            pSettings.Load(DefaultsCoP, true);
            pSettings.Load(DefaultsSoC);
            pSettings.Load(MtLoad);
            pSettings.Load(GameMtlPath);
            pSettings.Load(ProgressiveMeshes);
            pSettings.Load(StripifyMeshes);
            pSettings.Load(OptimizeSurfaces);
            pSettings.Load(SoCInfluence);
            pSettings.Load(SplitNormalsChbx, true);
            pSettings.Load(BuildInMotionsExport, true);
            pSettings.Load(SmoothSoC);
            pSettings.Load(SmoothCoP, true);
            pSettings.Load(Anims8Bit);
            pSettings.Load(Anims16Bit, !NoCompress.Checked);
            pSettings.Load(AnimsNoCompress, NoCompress.Checked);
            pSettings.Load(NoCompressLinkLabel);
            pSettings.Load(HQGeometry);
            pSettings.Load(HQGeometryPlus, true);
            pSettings.Load(ScaleCenterOfMassCheckBox, true);

            AnimsNoCompress.Enabled = NoCompress.Checked;
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            NoCompressLinkLabel.LinkVisited = true;
            System.Diagnostics.Process.Start("https://github.com/mortany/stcop-engine/commit/7d6e967ff081f8f797890f2c1954ce493c8b7084");
        }

        private void DefaultsSoC_CheckedChanged(object sender, EventArgs e)
        {
            SoCInfluence.Checked = true;
            if (!NoCompress.Checked)
                Anims8Bit.Checked = true;
            SmoothSoC.Checked = true;
        }

        private void DefaultsCoP_CheckedChanged(object sender, EventArgs e)
        {
            SoCInfluence.Checked = false;
            if (!NoCompress.Checked)
                Anims16Bit.Checked = true;
            else
                AnimsNoCompress.Checked = true;
            SmoothCoP.Checked = true;
        }

        private void NoCompress_CheckedChanged(object sender, EventArgs e)
        {
            AnimsNoCompress.Enabled = (sender as CheckBox).Checked;

            if (AnimsNoCompress.Enabled)
                AnimsNoCompress.Checked = true;
            else
            {
                if (DefaultsSoC.Checked)
                    Anims8Bit.Checked = true;
                else
                    Anims16Bit.Checked = true;
            }

            Editor.SyncCompressUI(NoCompress.Checked);
        }

        private void SyncForm(object sender, EventArgs e)
        {
            Control control = sender as Control;
            bool CheckState = (sender is CheckBox) ? (sender as CheckBox).Checked : (sender as RadioButton).Checked;

            Control[] MainControls = null;
            MainControls = EditorForm.Controls.Find(control.Name, true);

            for (int i = 0; i < MainControls.Length; i++)
            {
                if (MainControls[i] is CheckBox)
                    (MainControls[i] as CheckBox).Checked = CheckState;
                else
                    (MainControls[i] as RadioButton).Checked = CheckState;
            }
        }

        private void ProgressiveCheck(object sender, EventArgs e)
        {
            CheckBox chbx = sender as CheckBox;

            if (chbx.Checked)
                StripifyMeshes.Checked = false;
        }

        private void StripifyCheck(object sender, EventArgs e)
        {
            CheckBox chbx = sender as CheckBox;

            if (chbx.Checked)
                ProgressiveMeshes.Checked = false;
        }
    }
}