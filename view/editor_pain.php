<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/gatekeeper.php');
//
require_once(full_path('models/html_handler.php'));
/**
 * 
 */
class EditorArea {
    private $href;
    private $proj_id;
    private $tmpl_root = 'resources/templates/';
    private $tab_list = null;

    function __construct($url,$proj_id) {
        $this->href = HTMLHandler::specialchars($url).'?id='.HTMLHandler::specialchars($proj_id);
        $this->proj_id = intval($proj_id);
    }

    public function makePage($tabs,$tmpls,$tmpl_map,$data_list) {
        $this->makeTabs($tabs);
        echo '<form action="'.$this->href.'" method="POST" id="editorArea">'.PHP_EOL;
        foreach ($tabs as $tbl_num => $tbl_name) {
            $tmpl_name = null;
            if (isset($tmpl_map[$tbl_name])) {
                $tmpl_name = $this->tmpl_root . sprintf('proj%03d/',$this->proj_id).$tmpls[$tmpl_map[$tbl_name]];
            }
            $this->makePain($tbl_num,$tmpl_name,$data_list[$tbl_name]);
        }
        echo '</form>'.PHP_EOL;
        // $this->makeFooter();
    }

    public function makeTabs($tabs) {
        $this->tab_list = $tabs;
        echo '<header class="tab-bar">'.PHP_EOL;
        echo '<a href="#" class="icon-list2 btn" title="サイドメニュー"></a>'.PHP_EOL;
        echo '<ul class="editor-tabs">'.PHP_EOL;
        foreach ($tabs as $tbl_num => $tbl_name) {
            echo '<li><a href="'.$this->href.'#tab'.HTMLHandler::specialchars($tbl_num).'" onclick="changeTab(this);">'.HTMLHandler::specialchars($tbl_name).'</a></li>'.PHP_EOL;
        }
        // echo '<a href="#" class="icon-new-tab btn"></a>'.PHP_EOL;
        echo '</ul>'.PHP_EOL;
        echo '</header>'.PHP_EOL;
    }

    public function makePain($tab_num,$tmpl_name,$data_list) {
        if (! $this->tab_list) {
            return;
        }
        // 以下は現状SNTRPG_Skills専用
        echo '<section id="tab'.HTMLHandler::specialchars($tab_num).'" class="tab-page">'.PHP_EOL;
        HTMLHandler::makeDatalist($this->tab_list[$tab_num],$data_list);
        if ($tmpl_name) {
            $tmpl = file_get_contents(full_path($tmpl_name));
            echo '<ol>'.PHP_EOL;
            foreach ($data_list as $data_row) {
                $tmpl_row = $tmpl;
                foreach ($data_row as $key => $value) {
                    if ($key == 'icon') {
                        $value = 'img/'.$value;
                    }
                    $tmpl_row = str_replace('{'.$key.'}',HTMLHandler::specialchars($value),$tmpl_row);
                }
                echo '<li>' . $tmpl_row . '</li>'.PHP_EOL;
            }
            echo '</ol>'.PHP_EOL;
        } else {
            $tmpl_name = $this->tmpl_root . 'default_template.php';
            include($tmpl_name);
        }
        echo '</section>'.PHP_EOL;
    }

    public function makeFooter() {
        echo '<input value="新規レコード" type="button"/>'.PHP_EOL;
        return;
    }
}
?>
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/editor_area.css')?>">
	<title>Editor on Browser</title>
</head>
<body>
<?php
if (isset($tmpl_list)) {
    $editor_area = new EditorArea($GLOBALS['URL'], $_GET['id']);
    $editor_area->makePage(
        $current_proj_tbl_list,
        $tmpl_list,
        $selected_tmpl,
        $current_proj_data_list
    );
}
?>
	<script src="<?=addFilemtime('js/editorClient.js')?>"></script>
</body>
</html>