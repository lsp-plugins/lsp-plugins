<?php
	$MENU = array( );
	
	function &find_menu_item($id, &$menu = null)
	{
		$result = null;
		global $MENU;
		if (!isset($id))
			return $result;
		if (!isset($menu))
			$menu = &$MENU;
		
		foreach ($menu as &$item)
		{
			if (isset($item['id']) && ($item['id'] == $id))
				return $item;

			if (isset($item['items']))
			{
				$subitem = &find_menu_item($id, $item['items']);
				if (isset($subitem))
					return $subitem;
			}
		}
	
 		return $result;
	}

	function def_menu_item($parent, $id, $text, $file = null)
	{
		global $MENU;
		
		$item = array(
					'id' => $id,
					'parent' => ((isset($parent)) && (strlen($parent) > 0)) ? $parent : null,
					'text' => $text,
					'root' => '',
					'path' => '',
					'file' => isset($file) ? $file : $id
				);
		
		if ((!isset($parent)) || (strlen($parent) <= 0))
		{
		    $item['root']    = '..';
			array_push($MENU, $item);
		}
		else
		{
			$mi = &find_menu_item($parent);
			$item['path']   .= ((strlen($mi['path']) > 0) ? $mi['path'] . '/' : '') . $mi['id'];
			$item['root']   .= '..' . ((strlen($mi['root']) > 0) ? '/' . $mi['root'] : '');
			
			if (!isset($mi['items']))
				$mi['items'] = array( $item );
			else
				array_push($mi['items'], $item);
		}
	}
	
	function output_menu(&$menu = null)
	{
		global $MENU;
		if (!isset($menu))
			$menu = &$MENU;
		
		print "<ul class=\"index\">\n";
		foreach ($menu as &$item)
		{
			if (isset($item['items']))
			{
				print("<li>${item['text']}</li>\n");
				output_menu($item['items']);
			}
			else
			{
				$path = (strlen($item['path']) > 0) ? $item['path'] . '/' : '';
				print("<li><a href=\"html/${path}${item['id']}.html\">${item['text']}</a></li>\n");
			}
		}
		print "</ul>\n";
	}
	
	function raw_menu_list(&$target, &$menu = null)
	{
		global $MENU;
		if (!isset($menu))
			$menu = &$MENU;
		
		foreach ($menu as &$item)
		{
			if (isset($item['items']))
				raw_menu_list($target, $item['items']);
			else
				array_push($target, $item);
		}
	}

?>
